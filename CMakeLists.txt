cmake_minimum_required(VERSION 3.16)

project(MyClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Sql)

# 自己添加的让他去UI文件里面找UI
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)


set(PROJECT_SOURCES
        src/main.cpp
        src/app/widget.cpp
        src/app/widget.h
        ui/widget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MyClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MyClient APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MyClient SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

        find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Core)

        add_executable(MyClient
            ${PROJECT_SOURCES}
            src/resources/Resource.qrc
            src/widgets/hoverbutton.h src/widgets/hoverbutton.cpp
            src/widgets/iplineedit.h src/widgets/iplineedit.cpp
            src/app/mainwindow.h src/app/mainwindow.cpp ui/mainwindow.ui
            src/utils/StyleLoader.h

            ui/components/mainwindow/chatListPage/chatlistpage.h ui/components/mainwindow/chatListPage/chatlistpage.cpp
            ui/components/mainwindow/friendListPage/friendlistpage.h ui/components/mainwindow/friendListPage/friendlistpage.cpp ui/components/mainwindow/friendListPage/friendlistpage.ui
            ui/components/mainwindow/userInfoPage/userinfopage.h ui/components/mainwindow/userInfoPage/userinfopage.cpp ui/components/mainwindow/userInfoPage/userinfopage.ui
            ui/components/mainwindow/addFriendPage/addfriendpage.h ui/components/mainwindow/addFriendPage/addfriendpage.cpp ui/components/mainwindow/addFriendPage/addfriendpage.ui
            ui/components/mainwindow/placeholderPage/placeholderpage.h ui/components/mainwindow/placeholderPage/placeholderpage.cpp ui/components/mainwindow/placeholderPage/placeholderpage.ui

            src/utils/appconfig.h src/utils/appconfig.cpp
            src/utils/comapi/myapp.cpp src/utils/comapi/myapp.h src/utils/comapi/qqcell.cpp src/utils/comapi/qqcell.h

            ui/components/friendListPage/qqcelltitle.h ui/components/friendListPage/qqcelltitle.cpp ui/components/friendListPage/qqcelltitle.ui
            ui/components/friendListPage/qqcellline.h ui/components/friendListPage/qqcellline.cpp ui/components/friendListPage/qqcellline.ui

            ui/components/friendListPage/friendlistwidget.h ui/components/friendListPage/friendlistwidget.cpp

            src/utils/utils.h
            ui/components/chatListPage/chatlist_main.h ui/components/chatListPage/chatlist_main.cpp ui/components/chatListPage/chatlist_main.ui

            ui/components/chatListPage/chat-widget/chatwidget.h ui/components/chatListPage/chat-widget/chatwidget.cpp
            ui/components/chatListPage/chat-widget/chatmodel.h ui/components/chatListPage/chat-widget/chatmodel.cpp
            ui/components/chatListPage/chat-widget/chatdelegate.h ui/components/chatListPage/chat-widget/chatdelegate.cpp
            ui/components/ProfilePage/profilepage_main.h ui/components/ProfilePage/profilepage_main.cpp ui/components/ProfilePage/profilepage_main.ui
            ui/components/addfrienddialog/addfrienddialog.h ui/components/addfrienddialog/addfrienddialog.cpp ui/components/addfrienddialog/addfrienddialog.ui
            ui/components/FriendNotify/friendnotify_page.h ui/components/FriendNotify/friendnotify_page.cpp ui/components/FriendNotify/friendnotify_page.ui
            ui/components/FriendNotify/fnp_line.h ui/components/FriendNotify/fnp_line.cpp ui/components/FriendNotify/fnp_line.ui
            ui/components/FriendNotify/FNPData.h

            ui/components/FriendSearchWidget/friendsearchwidget.h ui/components/FriendSearchWidget/friendsearchwidget.cpp ui/components/FriendSearchWidget/friendsearchwidget.ui
            ui/components/mainwindow/chatListPage/recent_chats/rc_line.h ui/components/mainwindow/chatListPage/recent_chats/rc_line.cpp ui/components/mainwindow/chatListPage/recent_chats/rc_line.ui
            ui/components/mainwindow/chatListPage/recent_data.h
            ui/components/Change_Avatar_Page/change_avatar_page.h ui/components/Change_Avatar_Page/change_avatar_page.cpp ui/components/Change_Avatar_Page/change_avatar_page.ui
            ui/components/Change_Avatar_Page/dragscrollarea.h
            ui/components/Change_Avatar_Page/dragscrollarea.cpp
            ui/components/Registration_Page/registration_page.h ui/components/Registration_Page/registration_page.cpp ui/components/Registration_Page/registration_page.ui
            ui/components/mainwindow/chatListPage/recent_time_utlis.h
            ui/components/mainwindow/chatListPage/model.h ui/components/mainwindow/chatListPage/model.cpp
            src/utils/AutoHideScrollbar/autohidescrollbar.h src/utils/AutoHideScrollbar/autohidescrollbar.cpp
            src/DataBaseManage/databasemanage.h src/DataBaseManage/databasemanage.cpp
            src/DataBaseManage/model/FriendInfo.h
            src/DataBaseManage/model/recent_messages.h
            src/DataBaseManage/model/ChatRecord.h
            src/utils/StyleLoader.cpp
            src/Network/socketclient.h src/Network/socketclient.cpp
            src/utils/comapi/Protocol.h
            src/Network/PacketProcessor/packetprocessor.h src/Network/PacketProcessor/packetprocessor.cpp
            src/Network/Service/authservice.h src/Network/Service/authservice.cpp
            src/Network/Service/servicemanager.h src/Network/Service/servicemanager.cpp
            src/utils/comapi/message_kind_utils.h
            src/DataBaseManage/model/FriendsResponse.h
            src/DataBaseManage/model/FriendsResponse.cpp
            src/Network/Service/avatarservice.h src/Network/Service/avatarservice.cpp
            src/Network/Handlers/handlerregistry.h src/Network/Handlers/handlerregistry.cpp
            src/Network/Files/tempchunkmanager.h src/Network/Files/tempchunkmanager.cpp








        )
    endif()
endif()

# 自己添加的，用于提升类的时候，路径报错的问题
target_include_directories(MyClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/components/Change_Avatar_Page
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/components/chatListPage/chat-widget
)

target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets  Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(MyClient PRIVATE Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MyClient)
endif()
set_target_properties(MyClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MyClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MyClient)
endif()
