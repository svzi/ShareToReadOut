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

import bb.cascades 1.2

NavigationPane {
    
    id: nav
    
    Page {
        titleBar: TitleBar {
            title: qsTr("Share To Read Out! (TTS)")
        }
        
        ScrollView {
            Container {
                topPadding: 20
                bottomPadding: 20
                leftPadding: 20
                rightPadding: 20
                
                Label {
                    text: qsTr("Text to Share:")
                }
                TextArea {
                    id: textareaText
                    text: "As I started the development of Read Out! (TTS), it wasn’t even been meant to be a separate app. At the beginning it was part of a ReadItNow! beta. In my previous blog post I already mentioned what ReadItNow! is, a native BlackBerry 10 client for the popular “read later” service Pocket. Because some users wanted to listen to articles, stored in ReadItNow!, rather than reading them, I’ve added a 3rd party app more than a year ago. Sadly that app had some limitations with large articles and the developer didn’t want to fix that issues. So I had a look at all available text to speech apps in the BlackBerry World. I couldn’t find anyone, that fitted with my needs. For that reason I decided to try it on my own… After some developing I was really happy with the result and I decided it was far better than everything else I’ve seen in the BlackBerry Word. So I extracted the working code from the ReadItNow! beta and put together a single app with only one purpose: bring TTS to a lot of apps! I simply don’t wanted to limit the usage to ReadItNow! as I thought other apps could benefit from this solution as well."
                    maxHeight: 510
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 2
                        }
                        horizontalAlignment: HorizontalAlignment.Fill
                        text: qsTr("Share Text")
                        onClicked: {
                            _app.invokeReadOutWithJSON(textareaText.text, offsetDropdown.selectedValue);
                        }
                    }
                    DropDown {
                        id: offsetDropdown
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 2
                        }
                        title: qsTr("Offset")
                        Option {
                            text: "0 s"
                            value: 0
                            selected: true
                        }
                        Option {
                            text: "1 s"
                            value: 1
                        }
                        Option {
                            text: "2 s"
                            value: 2
                        }
                        Option {
                            text: "3 s"
                            value: 3
                        }
                    }
                }
                
                Divider {
                
                }   
                Label {
                    text: qsTr("URL to Share:")
                }
                
                TextArea {
                    id: textareaURL
                    text: "http://blog.sven-ziegler.com/2015/09/25/text-to-speech-tts-integration-via-read-out-tts-on-blackberry-10/"
                }     
                
                Button {
                    horizontalAlignment: HorizontalAlignment.Fill
                    text: qsTr("Share URL")
                    onClicked: {
                        _app.invokeReadOutWithURL(textareaURL.text);
                    }
                }
            
            }
        }
    }
   
    attachedObjects: [
        ComponentDefinition {
            id: readOutAdvertise
            source: "AdvertiseReadOut.qml"
        }
    ]
    
    function showReadOutAd() {
        var page = readOutAdvertise.createObject();
        nav.push(page);
    }
    
    onCreationCompleted: {
        _app.readoutNotFound.connect(showReadOutAd);
    }
    
}
