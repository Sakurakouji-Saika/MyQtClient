cmake_minimum_required(VERSION 3.16)

project(MyClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

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
        add_executable(MyClient
            ${PROJECT_SOURCES}
            src/resources/Resource.qrc
            src/widgets/hoverbutton.h src/widgets/hoverbutton.cpp
            src/widgets/iplineedit.h src/widgets/iplineedit.cpp
            src/app/mainwindow.h src/app/mainwindow.cpp ui/mainwindow.ui
            src/utils/StyleLoader.h src/utils/StyleLoader.cpp


            ui/components/mainwindow/chatListPage/chatlistpage.h ui/components/mainwindow/chatListPage/chatlistpage.cpp ui/components/mainwindow/chatListPage/chatlistpage.ui
            ui/components/mainwindow/friendListPage/friendlistpage.h ui/components/mainwindow/friendListPage/friendlistpage.cpp ui/components/mainwindow/friendListPage/friendlistpage.ui
            ui/components/mainwindow/chatContentPage/chatcontentpage.h ui/components/mainwindow/chatContentPage/chatcontentpage.cpp ui/components/mainwindow/chatContentPage/chatcontentpage.ui
            ui/components/mainwindow/userInfoPage/userinfopage.h ui/components/mainwindow/userInfoPage/userinfopage.cpp ui/components/mainwindow/userInfoPage/userinfopage.ui
            ui/components/mainwindow/addFriendPage/addfriendpage.h ui/components/mainwindow/addFriendPage/addfriendpage.cpp ui/components/mainwindow/addFriendPage/addfriendpage.ui
            ui/components/mainwindow/placeholderPage/placeholderpage.h ui/components/mainwindow/placeholderPage/placeholderpage.cpp ui/components/mainwindow/placeholderPage/placeholderpage.ui
            src/utils/clientsocket.h src/utils/clientsocket.cpp
            src/utils/appconfig.h src/utils/appconfig.cpp
            src/utils/comapi/global.h src/utils/comapi/iteminfo.cpp src/utils/comapi/iteminfo.h src/utils/comapi/myapp.cpp src/utils/comapi/myapp.h src/utils/comapi/qqcell.cpp src/utils/comapi/qqcell.h src/utils/comapi/unit.h

            ui/components/contacts/qqcelltitle.h ui/components/contacts/qqcelltitle.cpp ui/components/contacts/qqcelltitle.ui
            ui/components/contacts/qqcellline.h ui/components/contacts/qqcellline.cpp ui/components/contacts/qqcellline.ui

            ui/components/contacts/friendlistwidget.h ui/components/contacts/friendlistwidget.cpp
            ui/components/contacts/FriendInfo.h
            src/utils/utils.h


        )
    endif()
endif()

# 自己添加的，用于提升类的时候，路径报错的问题
target_include_directories(MyClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
)

target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets  Qt${QT_VERSION_MAJOR}::Network)

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
