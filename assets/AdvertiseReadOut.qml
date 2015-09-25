import bb.cascades 1.0

Page {
    titleBar: TitleBar {
        title: qsTr("ReadItNow! not found")
    }
    ScrollView {
        Container {
            leftPadding: 10
            rightPadding: 10
            topPadding: 10
 
            Label {
                text: qsTr("This app supports 'Read Out! (TTS)' for adding stuff to your Pocket account.")
                multiline: true
            }
            Divider {

            }
            Container {
                rightPadding: 10
                leftPadding: 10
                topPadding: 20
                //bottomPadding: 20
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Top
                        ImageView {
                            verticalAlignment: VerticalAlignment.Center
                            imageSource: "asset:///images/readout.png"
                            preferredHeight: 190
                            preferredWidth: 190
                            maxWidth: preferredWidth
                            maxHeight: preferredHeight
                            minHeight: preferredHeight
                            minWidth: preferredWidth
                            scalingMethod: ScalingMethod.AspectFit
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Top
                        Label {
                            text: qsTr("The app just reads out your texts. Quick and reliable. You have your hands free and you can concentrate on other things.")
                            multiline: true
                        }
                        //topPadding: 10
                        rightPadding: 10
                        leftPadding: 20
                        Container {
                             preferredHeight: 30
                        }
                    }
                }
            }
            Divider {

            }
            Label {
                text: qsTr("This app is unable to find 'Read Out! (TTS)' installed on your device. For that reason it's not possible to store any articles to read them later.")
                multiline: true
            }
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                topPadding: 40
                bottomPadding: 40
                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    text: qsTr("View Read Out! (TTS)")
                    attachedObjects: [
                        Invocation {
                            id: invoke
                            query: InvokeQuery {
                                mimeType: "application/x-bb-appworld"
                                uri: "appworld://content/59966912"
                            }
                        }
                    ]
                    onClicked: {
                        invoke.trigger("bb.action.OPEN")
                    }
                }
            }
            Container {
                minHeight: 40
            }
        }
    }
}
