// ekke (Ekkehard Gentz) @ekkescorner
import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtGraphicalEffects 1.0
import "../common"

Flickable {
    id: flickable
    contentHeight: root.implicitHeight
    // StackView manages this, so please no anchors here
    // anchors.fill: parent
    property string name: "PageThree"
    property string title: qsTr("Subway")
    property string emailID : ""
    property  bool subscribed : false
    property string info : ""

    function disableSubmit() {
        submit.enabled = false
    }

    function showInfo(info) {
        popupInfo.text = info
        popupInfo.buttonText = qsTr("OK")
        popupInfo.open()

    }
    function log(emailID)
    {
        var data = {};
        data.email = emailID;
        var json = JSON.stringify(data);
        console.log(json);
        const url = "https://vkguptamantra.herokuapp.com/api/users/";

        var xhr = new XMLHttpRequest();
        xhr.open("POST", url, true);
        xhr.setRequestHeader('Content-type','application/json; charset=utf-8');
        xhr.onreadystatechange = function () {


            if(xhr.responseText === "")
            {
                showInfo("Unable to connect to server. \n No connection");
                return;
            }


            if (xhr.readyState == 4 && xhr.status == 200) {
                var users = xhr.responseText;
                users = JSON.parse(users);
                console.log("success");
                console.log(users["_id"]);
                showInfo("Subscribed");
                disableSubmit();

                // save to sqliteDB
                server_id = users["_id"];
                emailID = users["email"];
                dbman.subscribe_db(server_id, emailID);


            }
            else if(xhr.status == 422) {
                console.log("exist");
                showInfo("Already Subscribed with this email");
            }

            else {
                console.log("error");
                console.log(xhr.status);
                console.error(users["error"]);
            }
        }
        xhr.send(json);


    }


    Pane {
        id: root
        anchors.fill: parent
        ColumnLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            LabelHeadline {
                leftPadding: 10
                text: qsTr("Subscribe")
            }
            IconInactive {
                imageName: modelData.icon
                imageSize: 48
            }
            HorizontalDivider {}
            RowLayout {
                LabelSubheading {
                    topPadding: 6
                    bottomPadding: 6
                    leftPadding: 10
                    rightPadding: 10
                    wrapMode: Text.WordWrap
                    text: qsTr("Subscribe by providing you email ID \n\n")
                }
            }

            HorizontalDivider {
                RowLayout {
                    TextField{
                        id: email
                        placeholderText: qsTr("eg@gmail.com");
                    }
                    Button
                    {
                        id: submit
                        text: "submit"
                        onPressed: {
                            emailID = email.text
                            log(emailID)

                        }

                    }

                }
            }


            } // col layout
        } // pane root
        ScrollIndicator.vertical: ScrollIndicator { }

        // emitting a Signal could be another option
        Component.onDestruction: {
            cleanup()
        }
        // called immediately after Loader.loaded
        function init() {
            console.log(qsTr("Init done from Three [2]"))
        }
        // called from Component.destruction
        function cleanup() {
            console.log(qsTr("Cleanup done from Three [2]"))
        }
    } // flickable

