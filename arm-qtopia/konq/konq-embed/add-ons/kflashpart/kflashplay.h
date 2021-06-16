#ifndef __KFLASHPLAY_H
#define __KFLASHPLAY_H

#include <config.h>

#include <kapp.h>
#include <kparts/mainwindow.h>

#include "kflashpart.h"
 
class KToggleAction;

/**
 * This is the application "Shell".  It has a menubar, toolbar, and
 * statusbar but relies on the "Part" to do all the real work.
 *
 * @short Application Shell
 * @author Paul Chitescu <paulc-devel@null.ro>
 * @version 0.1
 */
class KFlashPlay : public KParts::MainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    KFlashPlay();

    /**
     * Default Destructor
     */
    virtual ~KFlashPlay();

    /**
     * Use this method to load whatever file/URL you have
     */
    void load( const KURL& url );

protected:
    /**
     * This method is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties( KConfig * );

    /**
     * This method is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties( KConfig * );

    virtual bool queryClose();

private slots:
    void optionsShowToolbar();
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();

private:
    void setupAccel();
    void setupActions();

private:
    KFlashPart *m_part;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // __KFLASHPLAY_H
