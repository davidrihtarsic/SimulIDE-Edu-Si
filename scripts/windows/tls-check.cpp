#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslSocket>
#include <QTextStream>
#include <QTimer>
#include <QUrl>

int main( int argc, char* argv[] )
{
    QCoreApplication app( argc, argv );
    QTextStream out( stdout );
    out << "Qt: " << qVersion() << Qt::endl;
    out << "TLS build: " << QSslSocket::sslLibraryBuildVersionString() << Qt::endl;
    out << "TLS runtime: " << QSslSocket::sslLibraryVersionString() << Qt::endl;
    if( !QSslSocket::supportsSsl() )
    {
        out << "ERROR: TLS initialization failed." << Qt::endl;
        return 1;
    }
    out << "TLS initialization: OK" << Qt::endl;
    if( argc < 2 ) return 0;

    // Optional real HTTPS request for diagnosing component downloads.
    // Keep certificate validation enabled, exactly as in the application.
    QUrl url( app.arguments().at(1) );
    if( !url.isValid() || url.scheme() != "https" ) return 2;
    QNetworkAccessManager manager;
    QNetworkRequest request( url );
    request.setAttribute( QNetworkRequest::RedirectPolicyAttribute,
                          QNetworkRequest::NoLessSafeRedirectPolicy );
    request.setTransferTimeout( 15000 );
    QNetworkReply* reply = manager.get( request );
    QTimer deadline;
    deadline.setSingleShot( true );
    QObject::connect( &deadline, &QTimer::timeout, reply, &QNetworkReply::abort );
    QObject::connect( reply, &QNetworkReply::finished, &app, [&]() {
        if( reply->error() == QNetworkReply::NoError )
        {
            out << "HTTPS: OK (" << reply->readAll().size() << " bytes)" << Qt::endl;
            app.exit(0);
        }
        else
        {
            out << "HTTPS error: " << reply->errorString() << Qt::endl;
            app.exit(3);
        }
    } );
    deadline.start( 20000 );
    return app.exec();
}
