// ekke (Ekkehard Gentz) @ekkescorner
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects 1.0

import "../common"


Flickable {
    id: flickable
    contentHeight: root.implicitHeight
    // StackView manages this, so please no anchors here
    // anchors.fill: parent
    property string name: "PageOne"
    property string title: qsTr("Car")
    property string image_url: ""

    function showInfo(info) {
        popupInfo.text = info
        popupInfo.buttonText = qsTr("OK")
        popupInfo.open()

    }
    function getImage_url()
    {
        image_url = dbman.getImage_url();
        if(image_url === "")
        {
            image_url = "http://vkguptamantra.herokuapp.com/images/Mantra1.png";
        }
        else
        {
            image_url = "http://vkguptamantra.herokuapp.com/images/" + image_url;
            console.log(image_url);

        }
        return image_url;
    }


    function parsing()
    {
        var http = new XMLHttpRequest();

        var json , parse , html , search;


        http.onreadystatechange = function(){


            /*
                      checks if the
                      request finished and response is ready ( readystate 4)
                      200: "OK"
                      */
            if(http.readyState == 4 && http.status == 200)
            {

                json = http.responseText;
                console.log(json);
                parse = JSON.parse(json);
                console.log(parse[0]["_id"]);

            }


        };
        http.open('GET',"http://vkguptamantra.herokuapp.com/api/users/");
        http.send();
    }


    Pane {
        id: root
        anchors.fill: parent
        ColumnLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            LabelHeadline {
                leftPadding: 10
                text: qsTr("Today's Mantra")
            }
            //            IconInactive {
            //                imageName: modelData.icon
            //                imageSize: 48
            //            }
            HorizontalDivider {


            }
            RowLayout {

                Image {
                    id: mantra_image
                    antialiasing: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    clip: false
                    fillMode: Image.PreserveAspectFit
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    //                    source: "http://vkguptamantra.herokuapp.com/images/Mantra1.png"
                    source: getImage_url()
                }
            }
            HorizontalDivider {}
            RowLayout {
                LabelBodySecondary {
                    bottomPadding: 6
                    leftPadding: 10
                    rightPadding: 10
                    wrapMode: Text.WordWrap
                    text: qsTr("Have you recited your mantra today ?")
                }
                Button {
                    id: yes
                    width: 100

                    text: "YES"
                    antialiasing: false
                    //Layout.preferredWidth: 169
                    Layout.fillHeight: false
                    Layout.fillWidth: true
                    font.bold: true
                    onClicked: {
                        showInfo("Good Job !!");
                        yes.enabled = false;

                    }

                }
                Button {
                    id: no
                    width: 100
                    text: "NO"
                    //Layout.preferredWidth: 169
                    Layout.fillWidth: true
                    font.bold: true
                    onClicked: {
                        showInfo("Try to be consistent !!");
                        no.enabled = false;

                    }
                }
            }
            HorizontalDivider {
                RowLayout {


                }


            }
            //            RowLayout {}

        } // col layout
    } // root
    ScrollIndicator.vertical: ScrollIndicator { }

    // emitting a Signal could be another option
    Component.onDestruction: {
        cleanup()
    }

    // called immediately after Loader.loaded
    function init() {
        console.log(qsTr("Init done from One [0]"))
        //parsing()
    }
    // called from Component.destruction
    function cleanup() {
        console.log(qsTr("Cleanup done from One [0]"))
    }
} // flickable
