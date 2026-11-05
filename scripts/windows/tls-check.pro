TEMPLATE = app
TARGET = tls-check
QT += core network
QT -= gui
CONFIG += console c++11
CONFIG -= app_bundle
SOURCES += $$PWD/tls-check.cpp
