#ifndef __kcompletionbox_h__
#define __kcompletionbox_h__

class KCompletionBox
{
public:
    bool isVisible() const { return false; }
    int currentItem() const { return -1; } // ### check if this is okay!!
};

#endif

