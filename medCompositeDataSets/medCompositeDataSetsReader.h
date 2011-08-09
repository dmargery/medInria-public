#ifndef MEDCOMPOSITEDATASETSREADER_H
#define MEDCOMPOSITEDATASETSREADER_H

#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

#include <dirTools.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsReader: public dtkAbstractDataReader {
    Q_OBJECT

public:

    medCompositeDataSetsReader(): is_zip_file(false),desc(0) { }

    virtual ~medCompositeDataSetsReader() { cleanup(); }

    void cleanup() {
        if (!desc)
            delete desc;
        desc = 0;
        if (!reader)
            delete reader;
        reader = 0;
        qDebug() << "Cleaning: " << tmpdir;
        if (is_zip_file)
            RemoveDirectory(QDir(tmpdir));
    }

    virtual QString description() const { return "Reader for composite data sets";              }
    virtual QStringList handled() const { return MedInria::medCompositeDataSetsBase::handled(); }
    
    static bool initialize() {
        return dtkAbstractDataFactory::instance()->registerDataReaderType("medCompositeDataSetsReader",MedInria::medCompositeDataSetsBase::handled(),create);
    }

    static dtkAbstractDataReader* create() { return new medCompositeDataSetsReader(); }

public slots:

    virtual bool canRead(const QString& file);

    virtual bool read(const QString& file);
    virtual void readInformation(const QString& path);
    virtual void setProgress(const int value);

private:

    bool       is_zip_file;
    bool       in_error;
    QString    basedir;
    QString    tmpdir;
    QIODevice* desc;

    MedInria::medCompositeDataSetsBase* reader;
};

#endif
