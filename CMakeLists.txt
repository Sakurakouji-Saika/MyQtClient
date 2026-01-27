cmake_minimum_required(VERSION 3.16)

project(MyClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# 公共包含目录（按需调整）
set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets/avatar
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/components/Change_Avatar_Page
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/components/chatListPage/chat-widget
)

# ---------------------
# 把必须参与构建的 .cpp/.h 放到公共列表里，确保无论哪条分支都不会遗漏
# ---------------------
set(COMMON_SOURCES
    src/main.cpp
    src/app/widget.cpp
    src/app/widget.h
    ui/widget.ui
    src/utils/styleLoader.h
    src/utils/styleLoader.cpp
    src/utils/appconfig.h
    src/utils/appconfig.cpp
    src/widgets/hoverbutton.h
    src/widgets/hoverbutton.cpp

    # 确保 IPLineEdit 的实现被包含，避免 undefined reference
    src/widgets/iplineedit.h
    src/widgets/iplineedit.cpp

    # avatar / db / network 等常用模块也放公共列表（按需添加/删除）
    src/widgets/avatar/avatarlabel.h
    src/widgets/avatar/avatarlabel.cpp
    src/widgets/avatar/avatarmanager.h
    src/widgets/avatar/avatarmanager.cpp

    src/Network/socketclient.h
    src/Network/socketclient.cpp

    src/DataBaseManage/databasemanage.h
    src/DataBaseManage/databasemanage.cpp

    src/Network/Service/servicemanager.h src/Network/Service/servicemanager.cpp
    src/Network/Handlers/handlerregistry.h src/Network/Handlers/handlerregistry.cpp

    src/Network/Service/authservice.h
    src/Network/Service/authservice.cpp

    src/Network/Service/avatarservice.h
    src/Network/Service/avatarservice.cpp

    src/resources/Resource.qrc

    src/widgets/iplineedit.h
    src/widgets/iplineedit.cpp
    src/app/mainwindow.h
    src/app/mainwindow.cpp
    ui/mainwindow.ui

    ui/components/mainwindow/chatListPage/chatlistpage.h
    ui/components/mainwindow/chatListPage/chatlistpage.cpp

    ui/components/mainwindow/friendListPage/friendlistpage.h
    ui/components/mainwindow/friendListPage/friendlistpage.cpp
    ui/components/mainwindow/friendListPage/friendlistpage.ui

    ui/components/mainwindow/userInfoPage/userinfopage.h
    ui/components/mainwindow/userInfoPage/userinfopage.cpp
    ui/components/mainwindow/userInfoPage/userinfopage.ui

    ui/components/mainwindow/addFriendPage/addfriendpage.h
    ui/components/mainwindow/addFriendPage/addfriendpage.cpp
    ui/components/mainwindow/addFriendPage/addfriendpage.ui

    ui/components/mainwindow/placeholderPage/placeholderpage.h
    ui/components/mainwindow/placeholderPage/placeholderpage.cpp
    ui/components/mainwindow/placeholderPage/placeholderpage.ui

    src/utils/comapi/myapp.cpp
    src/utils/comapi/myapp.h
    src/utils/comapi/qqcell.cpp
    src/utils/comapi/qqcell.h

    ui/components/friendListPage/qqcelltitle.h
    ui/components/friendListPage/qqcelltitle.cpp
    ui/components/friendListPage/qqcelltitle.ui
    ui/components/friendListPage/qqcellline.h
    ui/components/friendListPage/qqcellline.cpp
    ui/components/friendListPage/qqcellline.ui

    ui/components/friendListPage/friendlistwidget.h
    ui/components/friendListPage/friendlistwidget.cpp

    src/utils/utils.h
    ui/components/chatListPage/chatlist_main.h
    ui/components/chatListPage/chatlist_main.cpp
    ui/components/chatListPage/chatlist_main.ui

    ui/components/chatListPage/chat-widget/chatwidget.h
    ui/components/chatListPage/chat-widget/chatwidget.cpp
    ui/components/chatListPage/chat-widget/chatmodel.h
    ui/components/chatListPage/chat-widget/chatmodel.cpp
    ui/components/chatListPage/chat-widget/chatdelegate.h
    ui/components/chatListPage/chat-widget/chatdelegate.cpp

    ui/components/ProfilePage/profilepage_main.h
    ui/components/ProfilePage/profilepage_main.cpp
    ui/components/ProfilePage/profilepage_main.ui

    ui/components/addfrienddialog/addfrienddialog.h
    ui/components/addfrienddialog/addfrienddialog.cpp
    ui/components/addfrienddialog/addfrienddialog.ui

    ui/components/FriendNotify/friendnotify_page.h
    ui/components/FriendNotify/friendnotify_page.cpp
    ui/components/FriendNotify/friendnotify_page.ui

    ui/components/FriendNotify/fnp_line.h
    ui/components/FriendNotify/fnp_line.cpp
    ui/components/FriendNotify/fnp_line.ui

    ui/components/FriendNotify/FNPData.h

    ui/components/FriendSearchWidget/friendsearchwidget.h
    ui/components/FriendSearchWidget/friendsearchwidget.cpp
    ui/components/FriendSearchWidget/friendsearchwidget.ui

    ui/components/mainwindow/chatListPage/recent_chats/rc_line.h
    ui/components/mainwindow/chatListPage/recent_chats/rc_line.cpp
    ui/components/mainwindow/chatListPage/recent_chats/rc_line.ui

    ui/components/mainwindow/chatListPage/recent_data.h

    ui/components/Change_Avatar_Page/change_avatar_page.h
    ui/components/Change_Avatar_Page/change_avatar_page.cpp
    ui/components/Change_Avatar_Page/change_avatar_page.ui
    ui/components/Change_Avatar_Page/dragscrollarea.h
    ui/components/Change_Avatar_Page/dragscrollarea.cpp

    ui/components/Registration_Page/registration_page.h
    ui/components/Registration_Page/registration_page.cpp
    ui/components/Registration_Page/registration_page.ui

    ui/components/mainwindow/chatListPage/recent_time_utlis.h
    ui/components/mainwindow/chatListPage/model.h
    ui/components/mainwindow/chatListPage/model.cpp

    src/utils/AutoHideScrollbar/autohidescrollbar.h
    src/utils/AutoHideScrollbar/autohidescrollbar.cpp

    src/DataBaseManage/databasemanage.h
    src/DataBaseManage/databasemanage.cpp
    src/DataBaseManage/model/FriendInfo.h
    src/DataBaseManage/model/recent_messages.h
    src/DataBaseManage/model/ChatRecord.h

    src/Network/socketclient.h
    src/Network/socketclient.cpp
    src/utils/comapi/Protocol.h
    src/Network/PacketProcessor/packetprocessor.h
    src/Network/PacketProcessor/packetprocessor.cpp


    src/utils/comapi/message_kind_utils.h
    src/DataBaseManage/model/FriendsResponse.h
    src/DataBaseManage/model/FriendsResponse.cpp


    src/Network/Files/tempchunkmanager.h
    src/Network/Files/tempchunkmanager.cpp
    src/widgets/avatar/avatarlabel.h
    src/widgets/avatar/avatarlabel.cpp
    src/widgets/avatar/avatarmanager.h
    src/widgets/avatar/avatarmanager.cpp
    src/DataBaseManage/ViewModel/FriendAvatarDTO.h
)

# Qt 库查找（支持 Qt6 或 Qt5）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Sql Core)
# 注意：为避免重复 find_package，后面按 major version 分支调用具体 Qt6/Qt5 targets

if (QT_VERSION_MAJOR GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Widgets Network Sql Core)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

    qt_add_executable(MyClient
        MANUAL_FINALIZATION
        ${COMMON_SOURCES}
        src/Network/Service/friendservice.h src/Network/Service/friendservice.cpp
        src/Network/models/userinfo.h
        src/Network/Handlers/appeventbus.h src/Network/Handlers/appeventbus.cpp
    )

    # 如果有 .qrc，推荐使用 qt_add_resources（Qt6）
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/Resource.qrc")
        qt_add_resources(MyClient "resources" "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/Resource.qrc")
    endif()

    # Qt6 特有的额外源（若有）可以追加
    target_sources(MyClient PRIVATE
        # e.g. src/qt6_only_file.cpp
    )

    target_link_libraries(MyClient PRIVATE Qt6::Widgets Qt6::Network Qt6::Sql Qt6::Core)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Network Sql Core)

    # Qt5 分支：在 Qt5 下可能需要加入许多额外 UI / 源文件
    list(APPEND COMMON_SOURCES

    )

    add_executable(MyClient ${COMMON_SOURCES})

    # 如果你还有 Qt5-only 的源，可以用 target_sources 追加
    target_sources(MyClient PRIVATE
        # e.g. src/qt5_only_file.cpp
    )

    target_link_libraries(MyClient PRIVATE Qt5::Widgets Qt5::Network Qt5::Sql Qt5::Core)
target_link_libraries(MyClient PRIVATE Qt${QT_VERSION_MAJOR}::Core)
endif()

# 公共 target 设置
target_include_directories(MyClient PRIVATE ${PROJECT_INCLUDE_DIRS})


set(QWindowKit_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/qwindowkit/lib/cmake/QWindowKit")
find_package(QWindowKit COMPONENTS Core Widgets Quick REQUIRED)
if (TARGET QWindowKit::Widgets)
    target_link_libraries(MyClient PRIVATE QWindowKit::Widgets)
endif()



target_compile_features(MyClient PRIVATE cxx_std_17)

# mac/win 打包属性（保留）
if (NOT DEFINED BUNDLE_ID_OPTION AND QT_VERSION VERSION_LESS "6.1.0")
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

if (QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_finalize_executable(MyClient)
endif()
