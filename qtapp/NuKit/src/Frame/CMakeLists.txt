AUX_SOURCE_DIRECTORY(. LIST_FRAME_LIB)
# This is the standard cmake command which keep the (to the slot) functionality work in the Qt Creator,
# please follow the convention
QT5_WRAP_UI(MAINWINDOW_UI_HDRS   mainwindow.ui)
ADD_LIBRARY(Frame ${LIST_FRAME_LIB} ${MAINWINDOW_UI_HDRS})
TARGET_LINK_LIBRARIES(Frame Qt5::DataVisualization)
TARGET_LINK_LIBRARIES(Frame Qt5::Widgets )
TARGET_LINK_LIBRARIES(Frame Qt5::Script) # For ScriptEngine in mainwindow.hpp
