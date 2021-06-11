#ifndef SL_BTMESH_BUTTON_PRESS_CONFIG_H_
#define SL_BTMESH_BUTTON_PRESS_CONFIG_H_

// <<< Use Configuration Wizard in Context Menu >>>

// <o SHORT_BUTTON_PRESS_DURATION> Duration of Short button presses
// <i> Default: 250
// <i> Any button press shorter than this value will be considered SHORT_BUTTON_PRESS.
#define SHORT_BUTTON_PRESS_DURATION   (250)

// <o MEDIUM_BUTTON_PRESS_DURATION> Duration of MEDIUM button presses
// <i> Default: 1000
// <i> Any button press shorter than this value and longer SHORT_BUTTON_PRESS than will
// be considered MEDIUM_BUTTON_PRESS.
#define MEDIUM_BUTTON_PRESS_DURATION   (1000)

// <o LONG_BUTTON_PRESS_DURATION> Duration of LONG button presses
// <i> Default: 5000
// <i> Any button press shorter than this value and longer MEDIUM_BUTTON_PRESS than will
// be consideredLONG_BUTTON_PRESS. Any button press longer than this value will be
// considered VERY_LONG_BUTTON_PRESS
#define LONG_BUTTON_PRESS_DURATION   (5000)

// <<< end of configuration section >>>

#endif // SL_BTMESH_BUTTON_PRESS_CONFIG_H_
