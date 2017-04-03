cmake_minimum_required(VERSION 3.6.0)
project(ufocr C CXX)

#set(CMAKE_C_COMPILER /media/zamazan4ik/For_Linux/OpenSource/clang_trunk/bin/clang)
#set(CMAKE_CXX_COMPILER /media/zamazan4ik/For_Linux/OpenSource/clang_trunk/bin/clang++)

#set(CMAKE_C_COMPILER /usr/bin/clang)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)

#set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2 -g -fsanitize=address -fno-omit-frame-pointer -shared-libasan" )
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2" )

include_directories(${ASPELL_INCLUDE_DIR} ${Qt5_INCLUDE_DIR} include/opencv include/opencv/opencv2 include/spdlog)

link_directories(lib/opencv lib/libjpeg lib/libpng lib/libtiff lib/zlib lib/libjasper
        lib/libwebp lib/libippcv lib/libprotobuf lib/leptonica
        lib/libgif)


#set(QT_QMAKE_EXECUTABLE /usr/bin/qmake)
#if (NOT DEFINED  ${CMAKE_INSTALL_PREFIX})
#set(CPACK_INSTALL_PREFIX /usr/)
#set (CMAKE_INSTALL_PREFIX /usr/)
#endif()

#set(CMAKE_MODULE_PATH "/home/zamazan4ik/Soft/Qt5.6.2/5.6/gcc_64/lib/cmake/")
#set(Qt5_DIR "/home/zamazan4ik/Soft/Qt5.6.2/5.6")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)

set(TRANSLATIONS
        translations/yagf_ru.ts
        translations/yagf_fr.ts
        translations/yagf_uk.ts
        )

set(OpenCV_LIBSS libopencv_ximgproc.a libopencv_xphoto.a libopencv_shape.a libopencv_imgcodecs.a libopencv_ts.a
        libopencv_videostab.a libopencv_photo.a libopencv_calib3d.a
        libopencv_video.a libopencv_stitching.a
        libopencv_videoio.a libopencv_superres.a
        libopencv_ml.a libopencv_objdetect.a  libopencv_features2d.a
        libopencv_flann.a libopencv_imgproc.a libopencv_highgui.a libopencv_core.a)

#set (LIB_PATH_SUFFIX )

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (EXISTS "${CMAKE_INSTALL_PREFIX}lib64")
        set(LIB_PATH_SUFFIX 64)
    endif (EXISTS "${CMAKE_INSTALL_PREFIX}lib64")
    set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
add_definitions( -DPRILIBRARY_PATH="${CMAKE_INSTALL_PREFIX}lib${LIB_PATH_SUFFIX}/yagf/")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

# following 2 lines define where interface translations will be installed
set(QML_DESTINATION share/yagf/translations/)
set(QML_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
add_definitions( -DQML_INSTALL_PATH="${QML_INSTALL_PREFIX}${QML_DESTINATION}")

set(SOURCES src/main.cpp src/mainform.cpp src/qgraphicsinput.cpp src/utils.cpp src/qxtunixsignalcatcher.cpp src/spellchecker.cpp
    src/SkewAnalysis.cpp src/core/rotationcropper.cpp src/qxtgraphicsproxywidget.cpp src/popplerdialog.cpp src/pdfextractor.cpp src/pdf2ppt.cpp src/ghostscr.cpp
    src/configdialog.cpp src/core/ccbuilder.cpp src/core/analysis.cpp src/qsnippet.cpp src/sidebar.cpp src/droplabel.cpp src/projectmanager.cpp src/settings.cpp
    src/texteditor.cpp src/tblock.cpp src/page.cpp src/tpagecollection.cpp src/scanner.cpp src/core/imageprocessor.cpp src/recognitiondialog.cpp
    src/langselectdialog.cpp src/busyform.cpp src/tiffimporter.cpp src/QBusyIndicator.cpp src/recognizerwrapper.cpp
    src/core/binarize.cpp src/core/qipblackandwhiteimage.cpp src/core/qipgrayscaleimage.cpp src/core/subimagepp.cpp
    src/core/tableanalizer.cpp src/core/pageanalysis.cpp src/extprocessdialog.cpp src/djvu2pdf.cpp
    src/projectfiles.cpp src/saveprojectdialog.cpp src/fileobjectitem.cpp src/loadprojectdialog.cpp src/autosaver.cpp src/waitwindow.cpp src/globallock.cpp
    src/core/GeneralImage.cpp src/core/GeneralImage.hpp src/core/cvmatandqimage.cpp src/core/cvmatandqimage.h src/logger.hpp src/logger.cpp src/core/BlurDetection.hpp
    src/core/BlurDetection.cpp src/core/Thinning.cpp src/core/Thinning.hpp src/core/Smoothing.cpp src/core/Smoothing.hpp src/core/ColorBalance.cpp src/core/ColorBalance.hpp src/core/Warp.cpp src/core/Warp.hpp src/core/Deskew.cpp src/core/Deskew.hpp src/core/Cropping.cpp src/core/Cropping.hpp src/core/Binarization.cpp src/core/Binarization.hpp src/SystemTray.cpp src/SystemTray.hpp)

set(HEADERS src/settings.h )

set(INT_SOURCES src/preload.c)

set(UIS src/mainform.ui src/popplerdialog.ui src/configdialog.ui src/langselectdialog.ui src/busyform.ui src/recognitiondialog.ui src/extprocessdialog.ui
    src/saveprojectdialog.ui src/waitwindow.ui)

set(MOC_HEADERS src/mainform.h src/qxtunixsignalcatcher.h src/qxtunixscinternal.h src/qgraphicsinput.h src/qxtgraphicsproxywidget.h src/qxtgraphicsview.h
    src/popplerdialog.h src/pdfextractor.h src/configdialog.h src/core/ccbuilder.h src/sidebar.h
    src/droplabel.h src/projectmanager.h src/texteditor.h src/page.h src/tpagecollection.h src/scanner.h src/core/imageprocessor.h
    src/langselectdialog.h src/busyform.h src/tiffimporter.h src/QBusyIndicator.h src/recognizerwrapper.h
    src/recognizerwrapperinternal.h src/recognitiondialog.h src/extprocessdialog.h src/djvu2pdf.h src/saveprojectdialog.h
    src/loadprojectdialog.h src/menuaction.h src/autosaver.h src/waitwindow.h)

set(yagf_RCCS src/yagf.qrc)

set(QM_FILES ${CMAKE_BINARY_DIR}/yagf_uk.qm ${CMAKE_BINARY_DIR}/yagf_ru.qm ${CMAKE_BINARY_DIR}/yagf_fr.qm)

set(CMAKE_BUILD_TYPE Debug)

#Test env variables
message(STATUS ${CMAKE_BINARY_DIR})
message(STATUS ${PROJECT_SOURCE_DIR})


add_definitions(-Wall -g)

find_package(Qt5LinguistTools)
find_package(Qt5 REQUIRED Core Gui Widgets Concurrent PrintSupport)
find_package(ASPELL REQUIRED)

#include(${QT_USE_FILE})
#qt5_wrap_ui(UI_HEADERS ${UIS})
#qt5_wrap_cpp(MOC_SRCS ${MOC_HEADERS})
qt5_add_resources(yagf_RCC_SRCS ${yagf_RCCS})

include_directories( ${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
add_executable(yagf ${SOURCES} ${MOC_HEADERS} ${UI_HEADERS} ${MOC_SRCS} ${yagf_RCC_SRCS} ${QM_FILES})
add_library(xspreload SHARED ${INT_SOURCES})

target_link_libraries(xspreload ${CMAKE_DL_LIBS})
#target_link_libraries(yagf Qt5::Widgets Qt5::Gui Qt5::Concurrent Qt5::Core ${ASPELL_LIBRARIES} -fsanitize=address -shared-libasan -lpthread)
target_link_libraries(yagf Qt5::Widgets Qt5::Gui Qt5::Concurrent Qt5::Core Qt5::PrintSupport ${ASPELL_LIBRARIES} ${OpenCV_LIBSS}
        libleptonica.a

        #liblibjpeg.a
        libzlib.a
        liblibtiff.a
        libippicv.a
        liblibwebp.a
        liblibjasper.a

        libjpeg.a
        liblibpng.a
        libgif.a

        -lgomp
        -fPIC
        -lpthread)

add_custom_target(translations DEPENDS ${QM_FILES})
qt5_add_translation(${CMAKE_BINARY_DIR}/yagf_ru.qm ${PROJECT_SOURCE_DIR}/src/mainform.cpp ${PROJECT_SOURCE_DIR}/src/popplerdialog.cpp ${MOC_SRCS} ${UI_HEADERS} ${PROJECT_SOURCE_DIR}/src/translations/yagf_ru.ts)
qt5_add_translation(${CMAKE_BINARY_DIR}/yagf_fr.qm ${PROJECT_SOURCE_DIR}/src/mainform.cpp ${PROJECT_SOURCE_DIR}/src/popplerdialog.cpp ${MOC_SRCS} ${UI_HEADERS} ${PROJECT_SOURCE_DIR}/src/translations/yagf_fr.ts)
qt5_add_translation(${CMAKE_BINARY_DIR}/yagf_uk.qm ${PROJECT_SOURCE_DIR}/src/mainform.cpp ${PROJECT_SOURCE_DIR}/src/popplerdialog.cpp ${MOC_SRCS} ${UI_HEADERS} ${PROJECT_SOURCE_DIR}/src/translations/yagf_uk.ts)
#set(CMAKE_INSTALL_PREFIX /usr/)

#install(TARGETS yagf xspreload RUNTIME DESTINATION bin LIBRARY DESTINATION lib${LIB_PATH_SUFFIX}/yagf)
#install(FILES ${QM_FILES} DESTINATION ${QML_DESTINATION})
#install(FILES COPYING DESCRIPTION README AUTHORS ChangeLog DESTINATION share/yagf/)
#install(FILES yagf.png DESTINATION share/pixmaps/)
#install(FILES yagf.png DESTINATION share/icons/hicolor/96x96/apps/)
#install(FILES YAGF.desktop DESTINATION share/applications/)
#if (EXISTS /usr/share/appdata/)
#        install(FILES YAGF.appdata.xml DESTINATION /usr/share/appdata/)
#endif (EXISTS /usr/share/appdata/)

#[[set(CPACK_PACKAGE_FILE_NAME ufocr)
set(CPACK_PACKAGE_VENDOR "Alexander Zaitsev, zamazan4ik@tut.by")
set(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})
set(CPACK_SYSTEM_NAME "i586")
set(CPACK_BINARY_DEB ON)
set(CPACK_BINARY_RPM ON)
set(CPACK_BINARY_STGZ ON)
set(CPACK_BINARY_TGZ OFF)
set(CPACK_BINARY_TZ OFF)
set(CPACK_BINARY_TBZ2 OFF)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "User-Friendly OCR")
set(CPACK_PACKAGE_VERSION 0.0.1)
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_STRIP_FILES bin/yagf bin/libyagfpreload)
set(CPACK_SOURCE_TBZ2 "OFF")
set(CPACK_SOURCE_TGZ "ON")
set(CPACK_SOURCE_TZ "OFF")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${CMAKE_SOURCE_DIR}/yagf-${CPACK_PACKAGE_VERSION}/;/")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/DESCRIPTION)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)
set(DEBIAN_PACKAGE_SECTION "text processing")
set(CPACK_RPM_PACKAGE_LICENSE "GPL v.3.0")
set(CPACK_RPM_PACKAGE_REQUIRES "libqt > 5.5")
include(CPack)]]


