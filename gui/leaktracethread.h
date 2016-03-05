
#pragma once

#include <QThread>
#include <QString>
#include <QList>

struct ApiTraceError;

class LeakTraceThread: public QThread{

    Q_OBJECT

    public:

        LeakTraceThread(QString _filename):filename(_filename){}


        bool hasError()const {return error;}
    signals:

        void leakTraceErrors(const QList<ApiTraceError> &errors);

    protected:

        virtual void run() override;

    private:

        QString filename;

        bool error=false;
};


