#ifndef CB_H_
#define CB_H_

#include <stdint.h>
#define MAX_COMPONENTS 50

/**
 * Component Type
 * Core component unique type
 */
typedef enum
{
	BUTTON_TEXT = 0,
	BUTTON_IMAGE,
	BUTTON_TOGGLE,
	BUTTON_BACK_PANEL,
	SCROLL_LIST_VIEW,
    SCROLL_LIST_ITEM,
    SCROLL_GRID_VIEW,
    SCROLL_GRID_ITEM,
	NAVIGATION_TAB_VIEW,
	NAVIGATION_TAB_ITEM,
	DETAILS_VIEW_RIGHT_PANEL,
	DETAILS_VIEW_ADAPTER_INFO,
    DETAILS_VIEW_MODULE_INFO,
    DETAILS_VIEW_TITLE_INFO,
	RECTANGLE,
	COMPONENT_TYPE_NONE
} ComponentType;

/**
 * Component Focus Policy
 * FOCUSABLE - Component can receive focus
 * NON_FOCUSABLE - Component can not receive focus
 */
typedef enum
{
	FOCUSABLE = 0,
	NON_FOCUSABLE
} ComponentFocusPolicy;

/**
 * Current Component Focus state
 * UNFOCUSED - Component has focus
 * FOCUSED - Component not in focus
 */
typedef enum
{
	UNFOCUSED = 0,
	FOCUSED
} ComponentFocusState;

typedef enum {
    CURSOR_UP,
    CURSOR_DOWN,
    CURSOR_LEFT,
    CURSOR_RIGHT,
    SELECT,
    BACK,
    MAX_KEYPADKEYS
} KeypadKey;

/**
 * Current Component state
 *
 * STATE_OFF - Component is in Off state
 * STATE_ON - Component is in on state
 * STATE_DISABLED - Component is in disabled state
 *                  Can not receive focus/click
 * STATE_IDLE - Component in neutral state
 *
 */
typedef enum
{
    STATE_OFF = 0,
    STATE_ON,
    STATE_DISABLED,
    STATE_IDLE
} ComponentState;

/**
 * Scoll Direction
 */
typedef enum
{
    SCROLL_UP =0,
    SCROLL_DOWN,
    SCROLL_NONE
} ScrollDirection;

/**
 * @class ComponentBase
 *
 * Base class for all the UI components
 */
class ComponentBase
{
protected:
    // Component related x, y, width and height
    // boundingBox is also used in case of focus calculations
	Box boundingBox;

	// Set by the component or auto generated
	uint16_t componentId;

	// Component related properties
	ComponentType componentType;
    ComponentState componentState;
	ComponentFocusState componentFocusState;
	ComponentFocusPolicy componentFocusPolicy;

	// Pointer to driver
	RA8875_LCD_Driver* driver;

public:
	ComponentBase();
    ComponentBase (uint16_t id); // Component ID based
	virtual ~ComponentBase();

	/**
     * Called in case of focus change.
     * Component must redraw only focus update related changes
	 *
	 * @param
	 * @return void
	 */
	virtual void draw() {}

    /**
     * Called in case when full component need to be redrawn
     * Default behavior is call to "draw"
     *
     * @param
     * @return void
     */
    virtual void drawFull() { draw(); }

    /**
     * Method gets called, when data displayed in the screen is updated
     * And component needs to redraw only the data displayed in the screen
     * It wont update entire component to avoid flicker
     * Data referees to Text and Numbers displayed in the screen
     *
     * @param
     * @return void
     */
    virtual void redrawUpdatedData() { }

    /**
     * Set display driver for the component
     * Driver is updated when screen moves front/back
     * Overwrite this method for complex components
     *
     * @param d - Pointer to RA8875_LCD_Driver
     * @return void
     */
    virtual void setDisplayDriver (RA8875_LCD_Driver* d);

    /**
     * Method to get set Display Driver
     *
     * @param
     * @return RA8875_LCD_Driver pointer to driver
     */
    virtual RA8875_LCD_Driver* getDisplayDriver()
    {
    	return this->driver;
    }

    /**
     * Method to get '0' based index of selected list or grid element
     *
     * @param
     * @return int16_t - Default -1, In case of List or Grid index
                         of the selected or focused component
     */
    virtual int16_t getSelectedElementIndex() { return -1; }

    /**
     * Method to get component ID
     *
     * @param
     * @return uint16_t - ID of the component
     */
    virtual int16_t getComponentId() const;

    /**
     * Method to generate random component ID using "rand"
     * Method generates and sets the component ID
     *
     * @param
     * @return void
     */
    void generateComponentId();

    /**
     * Method to set component ID
     *
     * @param id - uint16_t unique ID of the component
     * @return void
     */
    void setComponentId(uint16_t id)
    {
        componentId = id;
    }

    /**
     * Method to get component focus policy
     *
     * @param
     * @return Enum ComponentFocusPolicy based policy
     */
	ComponentFocusPolicy getComponentFocusPolicy() const
	{
		return componentFocusPolicy;
	}

    /**
     * Method to set component focus policy
     *
     * @param policy - Enum ComponentFocusPolicy based policy
     * @return void
     */
	void setComponentFocusPolicy (ComponentFocusPolicy policy)
	{
		this->componentFocusPolicy = policy;
	}

    /**
     * Method to get component focus state
     *
     * @param
     * @return  Enum ComponentFocusState based state
     */
	ComponentFocusState getComponentFocusState() const
	{
		return componentFocusState;
	}

    /**
     * Method to get component state based on ComponentState enum
     * *List view overwrites to return list element related state
     *
     * @param
     * @return Enum ComponentState based state
     */
    virtual ComponentState getComponentState()
    {
        return componentState;
    }

    /**
     * Method to set component state based on ComponentState enum
     *
     * @param state - Enum ComponentState based state
     * @return void
     */
    void setComponentState (ComponentState state)
    {
        componentState = state;
    }

    /**
     * Method to get component type based on ComponentType enum
     *
     * @param
     * @return Enum ComponentType based type
     */
	ComponentType getComponentType() const
	{
		return componentType;
	}

    /**
     * Method to get component Bounding Box
     *
     * @param
     * @return Struct Box object
     */
	Box getBoundingBox() const
	{
		return boundingBox;
	}

    /**
     * Method to set component focus state
     * Method updates the focus state of the component
     * And also method "draw" called for updating state on the display
     *
     * @param state - enum ComponentFocusState
     * @return void
     */
	void setComponentFocusState (ComponentFocusState state)
    {
		this->componentFocusState = state;
		if (this->driver != NULL)
		{
		    draw(); // Draw updated state of the component on display
		}
	}

    /**
     * Method to print component related information
     * Information like: ID, X, Y, Width, Height and Focus state etc.
     *
     * @param
     * @return void
     */
	void dumpComponent() const
	{
		PRINT("Component ID: [%d] >> X:[%d]  Y:[%d]  W:[%d]  H:[%d]  S:[%d]\n",
               componentId, boundingBox.x, boundingBox.y,
               boundingBox.width, boundingBox.height,
               componentFocusState);
	}
};
#endif /* CB_H_ */
