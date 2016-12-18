#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QProcess psql;
    QFile logFile;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // log failis adgilmdebareoba
    QString configFileDirecctory = QString(env.value("appdata") + "/SoftGen/Lombard/Logs");
    QDir dir;
    // tu ar aris folderi maSin Seqmnis
    dir.mkpath(configFileDirecctory);
    //sad Seinaxos
    logFile.setFileName(configFileDirecctory + "/plsql.log");
    if (!logFile.open(QFile::WriteOnly | QFile::Append)) {
        qDebug() << "log failTan wvdomis Secdoma";
        return 0;
    }
    QTextStream log(&logFile);
    log.setCodec("UTF-8");
    log.setLocale(QLocale::Georgian);
    log.setAutoDetectUnicode(true);
    // log.setCodec("ANSI");
    log << "------------------------------------------------------\r\n" << QDateTime::currentDateTime().toString() << "\r\n";
    psql.setProcessChannelMode(QProcess::MergedChannels);
    //  psql.setProcessChannelMode(QProcess::ForwardedChannels);
    qDebug() << "ARGUMENTS COUNT - " << a.arguments().count() - 1;
    log << "ARGUMENTS COUNT - " << a.arguments().count() - 1 << "\r\n";
    for (int i = 1; i < a.arguments().count(); ++i)
        qDebug() << i << "\t" << a.arguments().at(i).toUtf8();

    /**
     * თუ არ არის არგუმენტი მაშინ გამოიტანს მხოლოდ ამას
     */
    if (a.arguments().count() == 1) {
        qDebug() << "version 0.0.0.1\nAutor - Giga Kokaia\n(c)2011 SoftGen Group";
        qDebug() << "NOTHING TO DO";
        log << QString::fromUtf8("version 0.0.0.1\r\nAutor - გიგა კოკაია\r\n©2011 SoftGen Group\r\n");
        log << "NOTHING TO DO\r\n";
        return 0;
    }

    /**
     * @note აქ არის ბაზიდან ბინარული ფაილის გამოტანა
     * @author GIGA KOKAIA
     * @param 1 = "GetBlob"
     * @param 2 = server ip
     * @param 3 = db name
     * @param 4 = user name
     * @param 5 = password
     * @param 6 = LOBOID
     * @param 7 = file name
     */
    if (a.arguments().at(1).compare("GetBlob", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 8) {
            qDebug() << QString("%1 - WRONG ARGUMENT COUNT").arg(a.arguments().count());
            log << QString("%1 - WRONG ARGUMENT COUNT\r\n").arg(a.arguments().count());
            return -1;
        }
        QString file_name = a.arguments().at(7);
        file_name.replace("\\", "/");
        if (QFile(file_name).exists()) {
            qDebug() << QString("FILE %1 EXIST").arg(a.arguments().at(7));
            log << QString::fromUtf8("ფაილი %1 არსებობს, მოხდება თავზე გადაწერა\r\n").arg(a.arguments().at(7));
        };
        qDebug() << "START GetBlob";

        QString params = QString("bin/psql -h %1 -U \"%2\" -w -d \"%3\" -c \"\\lo_export %4 '%5' \" ")
                .arg(a.arguments().at(2)).arg(a.arguments().at(4)).arg(a.arguments().at(3))
                .arg(a.arguments().at(6)).arg(file_name);
        qDebug() << params;
        env.insert("PGPASSWORD", a.arguments().at(5));
        psql.setProcessEnvironment(env);
        psql.start(params, QIODevice::ReadOnly);

        if (!psql.waitForStarted()) {
            qDebug() << " psql START ERROR" << psql.errorString();
            log << " psql START ERROR - " << psql.errorString();
            return -1;
        }
        if (!psql.waitForFinished(1800000)) {
            qDebug() << " psql Finish ERROR " << psql.errorString();
            log << " psql Finish ERROR - " << psql.errorString() << "\r\n";
            return -3;
        }
        QByteArray ba = psql.readAll();
        QTextStream in(ba);
        qDebug() << "GetBlob readAll \n" << ba;
        log << QString("GetBlob readAll - \r\n\"") << ba << "\"\r\n";
        QString str;
        in >> str;
        qDebug() << " str " << str;
        if (str.compare("lo_export", Qt::CaseInsensitive) == 0) {
            qDebug() << "Succeful";
            log << QString::fromUtf8("ფაილი %1 წარმატებით იქნა გადმოტანილი ბაზიდან, და ჩაიწერა %2 \r\n")
                    .arg(a.arguments().at(6)).arg(a.arguments().at(7));
        } else {
            qDebug() << "Unknown Error";
            log << "Unknown Error\r\n";
            return -5;
        }
        qDebug() << "psql.exitStatus() " << psql.exitStatus();
        log << "psql.exitStatus() " << psql.exitStatus();
        return psql.exitStatus();
    }



    /**
     * @note აქ არის ბაზაში ბინარული ფაილის წაშლა
     * @author GIGA KOKAIA
     * @param 1 = "DeleteBlob"
     * @param 2 = server ip
     * @param 3 = db name
     * @param 4 = user name
     * @param 5 = password
     * @param 6 = LOBOID
     */
    if (a.arguments().at(1).compare("DeleteBlob", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 6) {
            qDebug() << QString("%1 - WRONG ARGUMENT COUNT").arg(a.arguments().count());
            log << QString("%1 - WRONG ARGUMENT COUNT\r\n").arg(a.arguments().count());
            return -1;
        }
        qDebug() << "START DeleteBlob";
        QString params = QString("bin/psql -h %1 -U \"%2\" -w -d \"%3\" -c \"\\lo_unlink %4 \" ")
                .arg(a.arguments().at(2)).arg(a.arguments().at(4)).arg(a.arguments().at(3))
                .arg(a.arguments().at(6));
        qDebug() << params;
        env.insert("PGPASSWORD", a.arguments().at(5));
        psql.setProcessEnvironment(env);
        psql.start(params, QIODevice::ReadOnly);
        if (!psql.waitForStarted()) {
            qDebug() << " psql START ERROR" << psql.errorString();
            log << " psql START ERROR - " << psql.errorString();
            return -1;
        }
        if (!psql.waitForFinished(1800000)) {
            qDebug() << " psql Finish ERROR " << psql.errorString();
            log << " psql Finish ERROR - " << psql.errorString() << "\r\n";
            return -3;
        }
        QByteArray ba = psql.readAll();
        QTextStream in(ba);
        qDebug() << "DeleteBlob readAll \n" << ba;
        log << "DeleteBlob readAll - \r\n\"" << ba << "\"\r\n";
        QString str;
        in >> str;
        qDebug() << " str " << str;
        if (str.compare("lo_unlink", Qt::CaseInsensitive) == 0) {
            qint32 a;
            in >> a;
            qDebug() << " a = " << a;
            log << QString::fromUtf8("%1 წაშლილია").arg(a);
            return -a;
        } else {
            qDebug() << "Unknown Error";
            log << "Unknown Error\r\n";
            return -5;
        }
        qDebug() << "psql.exitStatus() " << psql.exitStatus();
        log << "psql.exitStatus() " << psql.exitStatus() << "\r\n";
        return psql.exitStatus();
    }


    /**
     * @note აქ არის ბაზაში ბინარული ფაილის შეტანა
     * @author GIGA KOKAIA
     * @param 1 = InsertBlob
     * @param 2 = server ip
     * @param 3 = db name
     * @param 4 = user name
     * @param 5 = password
     * @param 6 = file name
     * @param 7 = file comment
     */
    if (a.arguments().at(1).compare("InsertBlob", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 7) {
            qDebug() << QString("%1 - WRONG ARGUMENT COUNT").arg(a.arguments().count());
            log << QString("%1 - WRONG ARGUMENT COUNT\r\n").arg(a.arguments().count());
            return -1;
        }
        QString file_name = a.arguments().at(6);
        file_name.replace("\\", "/");
        if (!QFile(file_name).exists()) {
            qDebug() << QString("FILE %1 DONT EXIST").arg(a.arguments().at(6));
            log << QString("FILE %1 DONT EXIST\r\n").arg(a.arguments().at(6));
            return -4;
        };
        qDebug() << "START InsertBlob";
        QString params = QString("bin/psql -h %1 -U \"%2\" -w -d \"%3\" -c \"\\lo_import \'%4\' '%5'\"")
                .arg(a.arguments().at(2)).arg(a.arguments().at(4)).arg(a.arguments().at(3))
                .arg(file_name).arg(a.arguments().at(7));
        qDebug() << params;
        env.insert("PGPASSWORD", a.arguments().at(5));
        psql.setProcessEnvironment(env);
        psql.start(params, QIODevice::ReadOnly);
        if (!psql.waitForStarted()) {
            qDebug() << " psql START ERROR" << psql.errorString();
            log << " psql START ERROR - " << psql.errorString();
            return -1;
        }
        if (!psql.waitForFinished(1800000)) {
            qDebug() << " psql Finish ERROR " << psql.errorString();
            log << " psql Finish ERROR - " << psql.errorString() << "\r\n";
            return -3;
        }

        QByteArray ba = psql.readAll();
        QTextStream in(ba);
        qDebug() << "InsertBlob readAll \n" << ba;
        log << QString("InsertBlob readAll - \r\n\"") << ba << "\"\r\n";
        QString str;
        in >> str;
        qDebug() << " str " << str;
        if (str.compare("lo_import", Qt::CaseInsensitive) == 0) {
            qint32 a;
            in >> a;
            qDebug() << " a " << a;
            return -a;
        } else {
            qDebug() << "Unknown Error";
            log << "Unknown Error\r\n";
            return -5;
        }
        qDebug() << "psql.exitStatus() " << psql.exitStatus();
        log << "psql.exitStatus() " << psql.exitStatus();
        return psql.exitStatus();
    }


    /**
     * @def siis gamotana
     * @author GIGA KOKAIA
     * @param 1 = GetLis
     * @param 2 = server ip
     * @param 3 = db name
     * @param 4 = user name
     * @param 5 = password
     */
    if (a.arguments().at(1).compare("GetLis", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 5) {
            qDebug() << QString("%1 - WRONG ARGUMENT COUNT").arg(a.arguments().count());
            log << QString("%1 - WRONG ARGUMENT COUNT\r\n").arg(a.arguments().count());
            return -1;
        }
        QFile blobList;
        blobList.setFileName(configFileDirecctory + "/blobList.txt");
        if (!blobList.open(QFile::WriteOnly | QFile::Truncate)) {
            qDebug() << "blobList failTan wvdomis Secdoma";
            return 0;
        }
        QTextStream lst(&blobList);
        lst.setCodec("UTF-8");
        lst.setLocale(QLocale::Georgian);
        lst.setAutoDetectUnicode(true);
        qDebug() << "START GetLis";
        QString params = QString("bin/psql -h %1 -U \"%2\" -w -d \"%3\" -c \\lo_list ")
                .arg(a.arguments().at(2)).arg(a.arguments().at(4)).arg(a.arguments().at(3));
        qDebug() << params;
        env.insert("PGPASSWORD", a.arguments().at(5));
        psql.setProcessEnvironment(env);
        psql.start(params, QIODevice::ReadOnly);
        if (!psql.waitForStarted()) {
            qDebug() << " psql START ERROR" << psql.errorString();
            log << " psql START ERROR - " << psql.errorString();
            return -1;
        }
        if (!psql.waitForFinished(1800000)) {
            qDebug() << " psql Finish ERROR " << psql.errorString();
            log << " psql Finish ERROR - " << psql.errorString() << "\r\n";
            return -3;
        }

        QByteArray ba = psql.readAll();
        QTextStream in(ba);
        qDebug() << "GetLis readAll \n" << ba;
        log << QString("GetLis readAll - \r\n\"") << ba << "\"\r\n";
        QString line;
        line = in.readLine(); //            Large objects
        line = in.readLine(); //  ID   |    Owner    |  Description
        line = in.readLine(); //-------+-------------+----------------
        line = in.readLine();
        if (line.compare("(0 rows)", Qt::CaseInsensitive) == 0) {
            lst << 0;
            blobList.flush();
            return 0;
        }
        do {
            lst << line << "\r\n";
            line = in.readLine();
        } while (!line.isNull());

        /*
        QString str;
        in >> str;
        qDebug() << " str " << str;
        if (str.compare("lo_import", Qt::CaseInsensitive) == 0) {
            qint32 a;
            in >> a;
            qDebug() << " a " << a;
            return -a;
        } else {
            qDebug()  << "Unknown Error";
            log << "Unknown Error\r\n";
            return -5;
        }*/
        qDebug() << "psql.exitStatus() " << psql.exitStatus();
        log << "psql.exitStatus() " << psql.exitStatus();
        return psql.exitStatus();
    }

    /**
     * @def ბაზის აღდგენა
     * @author GIGA KOKAIA
     * @param 1 = RestoreBase
     * @param 2 = server ip
     * @param 3 = db name
     * @param 4 = user name
     * @param 5 = password
     * @param 6 = file name
     */
    if (a.arguments().at(1).compare("RestoreBase", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 7) {
            qDebug() << QString("%1 - WRONG ARGUMENT COUNT").arg(a.arguments().count());
            log << QString("%1 - WRONG ARGUMENT COUNT\r\n").arg(a.arguments().count());
            return -1;
        }
        qDebug() << "START Restore";
        QString params = QString("bin/pg_restore.exe --host %1 --port 5432 --username \"%2\" "
                " --dbname \"%3\" --no-password  --clean --single-transaction --verbose \"%4\""
                ).arg(a.arguments().at(2)).arg(a.arguments().at(4)).arg(a.arguments().at(3)).arg(a.arguments().at(6));
        qDebug() << params;
        env.insert("PGPASSWORD", a.arguments().at(5));
        psql.setProcessEnvironment(env);
        psql.start(params, QIODevice::ReadWrite);
        if (!psql.waitForStarted()) {
            qDebug() << " psql START ERROR" << psql.errorString();
            log << " psql START ERROR - " << psql.errorString();
            return -1;
        }
        if (!psql.waitForFinished(1800000)) {
            qDebug() << " psql Finish ERROR " << psql.errorString();
            log << " psql Finish ERROR - " << psql.errorString() << "\r\n";
            return -3;
        }
        return psql.exitStatus();
    }


    /**
     * @def ბაზის backup
     * @author GIGA KOKAIA
     * @param 1 = "dumpbase"
     * @param 2 = server ip
     * @param 3 = db name
     * @param 4 = user name
     * @param 5 = password
     * @param 6 = file name
     */
    if (a.arguments().at(1).compare("DumpBase", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 7) {
            qDebug() << QString("%1 - WRONG ARGUMENT COUNT").arg(a.arguments().count());
            log << QString("%1 - WRONG ARGUMENT COUNT\r\n").arg(a.arguments().count());
            return -1;
        }
        qDebug() << "START DUMPING";
        QString params = QString("bin/pg_dump.exe --host %1 --port 5432 --username \"%2\" --format custom --verbose  --no-password "
                " --blobs --encoding UTF8 --file \"%3\" \"%4\""
                ).arg(a.arguments().at(2)).arg(a.arguments().at(4)).arg(a.arguments().at(6)).arg(a.arguments().at(3));
        qDebug() << params;
        env.insert("PGPASSWORD", a.arguments().at(5));
        psql.setProcessEnvironment(env);
        psql.start(params, QIODevice::ReadWrite);
        if (!psql.waitForStarted()) {
            qDebug() << " psql START ERROR" << psql.errorString();
            log << " psql START ERROR - " << psql.errorString();
            return -1;
        }
        if (!psql.waitForFinished(1800000)) {
            qDebug() << " psql Finish ERROR " << psql.errorString();
            log << " psql Finish ERROR - " << psql.errorString() << "\r\n";
            return -3;
        }

        return psql.exitStatus();

    }


    /**
     * @note აქ არის TEST
     * @author GIGA KOKAIA
     * @param 1 = "sum"
     * @param 2 = Number
     * @param 3 = Number
     */
    if (a.arguments().at(1).compare("sum", Qt::CaseInsensitive) == 0) {
        if (a.arguments().count() < 3) {
            qDebug() << "WRONG ARGUMENT COUNT";
            log << "WRONG ARGUMENT COUNT";
            return -1;
        }
        QString params = QString("test sum %1 %2 ").arg(a.arguments().at(2)).arg(a.arguments().at(3));
        qDebug() << params << endl;
        psql.start(params, QIODevice::ReadWrite);
        /*
                if (!psql.waitForStarted()) {
                    qDebug() << " START ERROR" << psql.errorString();
                    return 0;
                }
                int r=4;
               // cin >> r;
                psql.write(QString::number(r).toUtf8());
                psql.closeWriteChannel();
                if (!psql.waitForBytesWritten()) {
                    qDebug() << " waitForBytesWritten " << psql.errorString();
                    return 0;
                }
         */
        if (!psql.waitForFinished())
            qDebug() << " test START ERROR" << psql.errorString();
        else
            qDebug() << " test OUT \n" << psql.readAll();
        qDebug() << psql.exitStatus();
    }


    /*
process.setProcessChannelMode(QProcess::MergedChannels);
//process.start("tracert", QStringList() << "google.ge");
process.start("ping", QStringList() << "localhost");
gzip.start("gzip", QStringList() << "-c");
if (!gzip.waitForStarted())
    return false;
gzip.write("Qt rocks!");
gzip.closeWriteChannel();
if (!gzip.waitForFinished())
    return false;
QByteArray result = gzip.readAll();
if (!process.waitForFinished())
    qDebug() << process.errorString();
else
    qDebug() << process.readAll();
QProcess* proc = new QProcess();
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
env.insert("PGPASSWORD", password);
proc->setProcessEnvironment(env);
proc->start("psql", QStringList() << "-U" << "postgres" << "-f" << initScriptFileName << lastDbName);
if (!proc->waitForStarted())
    app->showError(QObject::tr("Не удалось запустить psql"));
else
    if (proc->waitForFinished())
    lResult = true;

     */

    return 0;
    return a.exec();

}
