#include <stdlib.h>
#include "cb.h"

ComponentBase::ComponentBase()
{
	driver = NULL;
	boundingBox.x = 0;
    boundingBox.y = 0;
    boundingBox.width = 0;
    boundingBox.height = 0;

    componentType = COMPONENT_TYPE_NONE;
    componentFocusState = UNFOCUSED;
    componentFocusPolicy = NON_FOCUSABLE;
    componentState = STATE_IDLE;

    // Default ID is random number
    generateComponentId();
}

ComponentBase::ComponentBase (uint16_t id)
{
    driver = NULL;
    boundingBox.x = 0;
    boundingBox.y = 0;
    boundingBox.width = 0;
    boundingBox.height = 0;

    componentType = COMPONENT_TYPE_NONE;
    componentFocusState = UNFOCUSED;
    componentFocusPolicy = NON_FOCUSABLE;
    componentState = STATE_IDLE;

    // Set passed component ID
    componentId = id;
}

ComponentBase::~ComponentBase()
{
}

void ComponentBase::generateComponentId()
{
	componentId = rand() % MAX_COMPONENTS;
}

void ComponentBase::setDisplayDriver (RA8875_LCD_Driver* d)
{
	this->driver = d;
}

int16_t ComponentBase::getComponentId() const
{
    return this->componentId;
}
