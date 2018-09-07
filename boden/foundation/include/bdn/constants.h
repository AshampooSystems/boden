#ifndef BDN_Constants_H_
#define BDN_Constants_H_

namespace bdn
{

    /** Represents a tri-state (on, off, or mixed) for use with checkboxes and
     * toggle controls */
    enum TriState
    {
        /** The control is in off state (unchecked, toggled off) */
        off,
        /** The control is in on state (checked, toggled on) */
        on,
        /** The control is in mixed state (represents a combination of other
           on/off toggles, some of which are on and some of which are off
           currently) */
        mixed
    };
}

#endif // BDN_Constants_H_