/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

#include <bb/data/JsonDataAccess>

#include <bb/system/CardDoneMessage>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeQueryTargetsRequest>
#include <bb/system/InvokeQueryTargetsReply>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // make this class accessable from QML via _app property
    qml->setContextProperty("_app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("ShareToReadOut_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::check3rdPartyApps()
{
    InvokeManager *invokeManager = new InvokeManager(this);
    InvokeQueryTargetsRequest request;
    request.setMimeType("text/plain");
    const InvokeReply *reply = invokeManager->queryTargets(request);
    // Listen for the results
    bool ok = QObject::connect(reply, SIGNAL(finished()), this, SLOT(onCheck3rdPartyAppsResponse()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void ApplicationUI::onCheck3rdPartyAppsResponse()
{
   qDebug() << "onCheck3rdPartyAppsResponse()";

   // Get the reply from the sender object
   InvokeQueryTargetsReply *reply = qobject_cast<InvokeQueryTargetsReply*>(sender());
   if (reply) {
       if (reply->error() == InvokeReplyError::None) {
           QList<InvokeAction> targetsByAction = reply->actions();
           foreach (const InvokeAction &action, reply->actions()) {
               foreach (const InvokeTarget &target, action.targets()) {
                   //qDebug() << target.name();
                   if (target.name().indexOf("com.svzi.readout") != -1) {
                       m_readoutAvailable = true;
                   }
               }
           }
       } else if (reply->error() != InvokeReplyError::None) {
           qDebug() << "ERROR:" << reply->error();
       }

     // Clean up the results
     reply->deleteLater();
   }
   qDebug() << "m_readoutAvailable:" << m_readoutAvailable;
}

void ApplicationUI::invokeReadOutWithURL(const QString &url)
{
    if (m_readoutAvailable) {
        InvokeManager *invokeManager = new InvokeManager(this);
        bb::system::InvokeRequest request;

        request.setTarget("com.svzi.readout");
        request.setAction("bb.action.SHARE");
        request.setUri(url);

        invokeManager->invoke(request);
    } else {
        emit readoutNotFound();
    }

}

void ApplicationUI::invokeReadOutWithText(const QString &text)
{
   if (m_readoutAvailable) {// strip HTML code from content!
       QTextDocument cont;
       cont.setHtml(text);

       InvokeManager *invokeManager = new InvokeManager(this);
       bb::system::InvokeRequest request;

       request.setTarget("com.svzi.readout");
       request.setAction("bb.action.SHARE");
       request.setData(cont.toPlainText().toUtf8());

       invokeManager->invoke(request);
   } else {
       emit readoutNotFound();
   }
}

void ApplicationUI::invokeReadOutWithJSON(const QString &text, const int &position)
{
    if (m_readoutAvailable) {
        // prepare invocation request
        bb::system::InvokeManager *invokeManager = new bb::system::InvokeManager(this);
        bb::system::InvokeRequest request;
        request.setTarget("com.svzi.readout");
        request.setAction("bb.action.OPEN_FILE");
        request.setMimeType("text/file");

        // strip HTML code from content!
        QTextDocument cont;
        cont.setHtml(text);

        // create json file
        QVariantMap map;
        map.insert("text", cont.toPlainText());
        map.insert("positionInMilliseconds", position * 1000); // optional, to set a starting position where the read out should start
        map.insert("positionInPercent", 0); // optional, to set a starting position where the read out should start
        map.insert("title", "Test"); // optional, used for export file name of mp3
        map.insert("url", ""); // optinal, not used right now
        map.insert("image", ""); // optional, not used right now

        // create data file
        QDir a;
        a.setPath("/accounts/1000/clipboard");
        QFile file(a.path() + "/readout.json");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            JsonDataAccess jda(&file);
            jda.save(map, &file);
            if (jda.hasError()) {
                const DataAccessError err = jda.error();
                const QString errorMsg = tr("Error writing data to JSON file: %1").arg(err.errorMessage());
                qDebug() << errorMsg;
                file.close();
                return;
            }
            file.close();
        } else {
            qDebug() << "Could not open file for create!";
            return;
        }

        bool result = QObject::connect(invokeManager, SIGNAL(childCardDone(const bb::system::CardDoneMessage&)), this, SLOT(childCardDone(const bb::system::CardDoneMessage&)));
        Q_ASSERT(result);

        invokeManager->invoke(request);
    } else {
        emit readoutNotFound();
    }

}

void ApplicationUI::childCardDone(const bb::system::CardDoneMessage &doneMessage)
{
    // open the progress file
    QDir a;
    a.setPath("/accounts/1000/clipboard");
    QFile jsonFile(a.path() + "/readout_progress.json");

    if (!jsonFile.open(QFile::ReadOnly)) {
        qDebug() << "Could not find file for Read Out! (TTS)";
        return;
    } else {
        const QString doc = QString::fromUtf8(jsonFile.readAll());
        jsonFile.close();

        JsonDataAccess jda;
        QVariantMap progressMap = jda.loadFromBuffer(doc).toMap();
        if (jda.hasError()) {
            const DataAccessError err = jda.error();
            qDebug() << "Error converting data file for Read Out! (TTS):" << err.errorMessage();
            return;
        } else {
            // do your stuff here
            bool ok;
            double durationInMilliseconds = progressMap.value("durationInMilliseconds").toDouble(&ok);
            qDebug() << "durationInMilliseconds:" << durationInMilliseconds;
            double positionInPercent = progressMap.value("positionInPercent").toDouble(&ok);
            qDebug() << "positionInPercent:" << positionInPercent;
            double positionInMilliseconds = progressMap.value("positionInMilliseconds").toDouble(&ok);
            qDebug() << "positionInMilliseconds:" << positionInMilliseconds;

            if ((int) positionInPercent == 100) {
                // user has listened to everything
                qDebug() << "user heard 100%";
            } else {
                // user has stopped listened before it was over
                qDebug() << "user get bored or had another reason to stop listening";
            }
        }
   }
}
