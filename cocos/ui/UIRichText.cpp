/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "UIRichText.h"
#include "platform/CCFileUtils.h"
#include "2d/CCLabel.h"
#include "2d/CCSprite.h"
#include "base/ccUTF8.h"
#include "ui/UIHelper.h"

NS_CC_BEGIN

namespace ui {

    
bool RichElement::init(int tag, const Color3B &color, GLubyte opacity)
{
    _tag = tag;
    _color = color;
    _opacity = opacity;
    return true;
}
    
RichElementText* RichElementText::create(int tag, const Color3B &color, GLubyte opacity, const std::string& text, const std::string& fontName, float fontSize, uint32_t flags)
{
    RichElementText* element = new (std::nothrow) RichElementText();
    if (element && element->init(tag, color, opacity, text, fontName, fontSize, flags))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return nullptr;
}
    
bool RichElementText::init(int tag, const Color3B &color, GLubyte opacity, const std::string& text, const std::string& fontName, float fontSize, uint32_t flags)
{
    if (RichElement::init(tag, color, opacity))
    {
        _text = text;
        _fontName = fontName;
        _fontSize = fontSize;
		_flags = flags;
        return true;
    }
    return false;
}

RichElementImage* RichElementImage::create(int tag, const Color3B &color, GLubyte opacity, const std::string& filePath, const Size& size, const int textureType)
{
    RichElementImage* element = new (std::nothrow) RichElementImage();
    if (element && element->init(tag, color, opacity, filePath, size, textureType))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return nullptr;
}

bool RichElementImage::init(int tag, const Color3B &color, GLubyte opacity, const std::string& filePath, const Size& size, const int textureType)
{
    if (RichElement::init(tag, color, opacity))
    {
        _filePath = filePath;
		_size = size;
		_textureType = textureType;
        return true;
    }
    return false;
}

RichElementCustomNode* RichElementCustomNode::create(int tag, const Color3B &color, GLubyte opacity, cocos2d::Node *customNode)
{
    RichElementCustomNode* element = new (std::nothrow) RichElementCustomNode();
    if (element && element->init(tag, color, opacity, customNode))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return nullptr;
}
    
bool RichElementCustomNode::init(int tag, const Color3B &color, GLubyte opacity, cocos2d::Node *customNode)
{
    if (RichElement::init(tag, color, opacity))
    {
        _customNode = customNode;
        _customNode->retain();
        return true;
    }
    return false;
}

RichElementNewLine* RichElementNewLine::create(int tag, const Color3B& color, GLubyte opacity)
{
    RichElementNewLine* element = new (std::nothrow) RichElementNewLine();
    if (element && element->init(tag, color, opacity))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return nullptr;
}

RichText::RichText():
_formatTextDirty(true),
_leftSpaceWidth(0.0f),
_verticalSpace(0.0f)
{
    setCascadeOpacityEnabled(true);
}
    
RichText::~RichText()
{
    _richElements.clear();
}
    
RichText* RichText::create()
{
    RichText* widget = new (std::nothrow) RichText();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}
    
bool RichText::init()
{
    if (Widget::init())
    {
        return true;
    }
    return false;
}

void RichText::insertElement(RichElement *element, int index)
{
    _richElements.insert(index, element);
    _formatTextDirty = true;
}
    
void RichText::pushBackElement(RichElement *element)
{
    _richElements.pushBack(element);
    _formatTextDirty = true;
}
    
void RichText::removeElement(int index)
{
    _richElements.erase(index);
    _formatTextDirty = true;
}
    
void RichText::removeElement(RichElement *element)
{
    _richElements.eraseObject(element);
    _formatTextDirty = true;
}
    
void RichText::formatText()
{
    if (_formatTextDirty)
    {
        this->removeAllProtectedChildren();
        _elementRenders.clear();
        if (_ignoreSize)
        {
            addNewLine();
            for (ssize_t i=0, size = _richElements.size(); i<size; ++i)
            {
                RichElement* element = _richElements.at(i);
                Node* elementRenderer = nullptr;
                switch (element->_type)
                {
                    case RichElement::Type::TEXT:
                    {
                        RichElementText* elmtText = static_cast<RichElementText*>(element);
						Label* label;
                        if (FileUtils::getInstance()->isFileExist(elmtText->_fontName))
                        {
							label = Label::createWithTTF(elmtText->_text.c_str(), elmtText->_fontName, elmtText->_fontSize);
                        }
                        else
                        {
							label = Label::createWithSystemFont(elmtText->_text.c_str(), elmtText->_fontName, elmtText->_fontSize);
                        }
						if (elmtText->_flags & RichElementText::ITALICS)
							label->enableItalics();
						if (elmtText->_flags & RichElementText::BOLD)
							label->enableBold();
						if (elmtText->_flags & RichElementText::UNDERLINE)
							label->enableUnderline();
						if (elmtText->_flags & RichElementText::STRIKETHROUGH)
							label->enableStrikethrough();
						elementRenderer = label;
                        break;
                    }
                    case RichElement::Type::IMAGE:
                    {
                        RichElementImage* elmtImage = static_cast<RichElementImage*>(element);
						if (elmtImage->_textureType == (int)Widget::TextureResType::LOCAL)
							elementRenderer = Sprite::create(elmtImage->_filePath.c_str());
						else
							elementRenderer = Sprite::createWithSpriteFrameName(elmtImage->_filePath.c_str());
						if (elmtImage->_size.width != 0 && elmtImage->_size.height != 0)
						{
							auto currentSize = elementRenderer->getContentSize();
							elementRenderer->setScale(elmtImage->_size.width / currentSize.width, elmtImage->_size.height / currentSize.height);
							elementRenderer->setContentSize(elmtImage->_size);
						}
                        break;
                    }
                    case RichElement::Type::CUSTOM:
                    {
                        RichElementCustomNode* elmtCustom = static_cast<RichElementCustomNode*>(element);
                        elementRenderer = elmtCustom->_customNode;
                        break;
                    }
                    case RichElement::Type::NEWLINE:
                    {
                        addNewLine();
                        break;
                    }
                    default:
                        break;
                }

                if (elementRenderer)
                {
                    elementRenderer->setCascadeOpacityEnabled(true);
                    elementRenderer->setColor(element->_color);
                    elementRenderer->setOpacity(element->_opacity);
                    pushToContainer(elementRenderer);
                }
            }
        }
        else
        {
            addNewLine();
            for (ssize_t i=0, size = _richElements.size(); i<size; ++i)
            {
                RichElement* element = static_cast<RichElement*>(_richElements.at(i));
                switch (element->_type)
                {
                    case RichElement::Type::TEXT:
                    {
                        RichElementText* elmtText = static_cast<RichElementText*>(element);
                        handleTextRenderer(elmtText->_text.c_str(), elmtText->_fontName.c_str(), elmtText->_fontSize, elmtText->_color, elmtText->_opacity, elmtText->_flags);
                        break;
                    }
                    case RichElement::Type::IMAGE:
                    {
                        RichElementImage* elmtImage = static_cast<RichElementImage*>(element);
                        handleImageRenderer(elmtImage->_filePath.c_str(), elmtImage->_color, elmtImage->_opacity, elmtImage->_size, elmtImage->_textureType);
                        break;
                    }
                    case RichElement::Type::CUSTOM:
                    {
                        RichElementCustomNode* elmtCustom = static_cast<RichElementCustomNode*>(element);
                        handleCustomRenderer(elmtCustom->_customNode);
                        break;
                    }
                    case RichElement::Type::NEWLINE:
                    {
                        addNewLine();
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        formatRenderers();
        _formatTextDirty = false;
    }
}
    
void RichText::handleTextRenderer(const std::string& text, const std::string& fontName, float fontSize, const Color3B &color, GLubyte opacity, uint32_t flags)
{
    auto fileExist = FileUtils::getInstance()->isFileExist(fontName);
    Label* textRenderer = nullptr;
    if (fileExist)
    {
        textRenderer = Label::createWithTTF(text, fontName, fontSize);
    } 
    else
    {
        textRenderer = Label::createWithSystemFont(text, fontName, fontSize);
    }
    float textRendererWidth = textRenderer->getContentSize().width;
    _leftSpaceWidth -= textRendererWidth;
    if (_leftSpaceWidth < 0.0f)
    {
        float overstepPercent = (-_leftSpaceWidth) / textRendererWidth;
        std::string curText = text;
        size_t stringLength = StringUtils::getCharacterCountInUTF8String(text);
        int leftLength = stringLength * (1.0f - overstepPercent);
        std::string leftWords = Helper::getSubStringOfUTF8String(curText,0,leftLength);
        std::string cutWords = Helper::getSubStringOfUTF8String(curText, leftLength, stringLength - leftLength);
        if (leftLength > 0)
        {
            Label* leftRenderer = nullptr;
            if (fileExist)
            {
                leftRenderer = Label::createWithTTF(Helper::getSubStringOfUTF8String(leftWords, 0, leftLength), fontName, fontSize);
            }
            else
            {
                leftRenderer = Label::createWithSystemFont(Helper::getSubStringOfUTF8String(leftWords, 0, leftLength), fontName, fontSize);
            }
            if (leftRenderer)
            {
				if (flags & RichElementText::ITALICS)
					leftRenderer->enableItalics();
				if (flags & RichElementText::BOLD)
					leftRenderer->enableBold();
				if (flags & RichElementText::UNDERLINE)
					leftRenderer->enableUnderline();
				if (flags & RichElementText::STRIKETHROUGH)
					leftRenderer->enableStrikethrough();
                leftRenderer->setColor(color);
                leftRenderer->setOpacity(opacity);
                pushToContainer(leftRenderer);
            }
        }

        addNewLine();
        handleTextRenderer(cutWords.c_str(), fontName, fontSize, color, opacity, flags);
    }
    else
    {
		if (flags & RichElementText::ITALICS)
			textRenderer->enableItalics();
		if (flags & RichElementText::BOLD)
			textRenderer->enableBold();
		if (flags & RichElementText::UNDERLINE)
			textRenderer->enableUnderline();
		if (flags & RichElementText::STRIKETHROUGH)
			textRenderer->enableStrikethrough();
        textRenderer->setColor(color);
        textRenderer->setOpacity(opacity);
        pushToContainer(textRenderer);
    }
}
    
void RichText::handleImageRenderer(const std::string& filePath, const Color3B &/*color*/, GLubyte /*opacity*/, Size &size, int textureType)
{
	Sprite* imageRenderer = nullptr;
	if (textureType == (int)Widget::TextureResType::LOCAL)
		imageRenderer = Sprite::create(filePath.c_str());
	else
		imageRenderer = Sprite::createWithSpriteFrameName(filePath.c_str());
	if (imageRenderer)
	{
		if (size.width != 0 && size.height != 0)
		{
			auto currentSize = imageRenderer->getContentSize();
			imageRenderer->setScale(size.width / currentSize.width, size.height / currentSize.height);
			imageRenderer->setContentSize(size);
		}
		handleCustomRenderer(imageRenderer);
	}
}
    
void RichText::handleCustomRenderer(cocos2d::Node *renderer)
{
    Size imgSize = renderer->getContentSize();
    _leftSpaceWidth -= imgSize.width;
    if (_leftSpaceWidth < 0.0f)
    {
        addNewLine();
        pushToContainer(renderer);
        _leftSpaceWidth -= imgSize.width;
    }
    else
    {
        pushToContainer(renderer);
    }
}
    
void RichText::addNewLine()
{
    _leftSpaceWidth = _customSize.width;
    _elementRenders.push_back(new Vector<Node*>());
}
    
void RichText::formatRenderers()
{
    if (_ignoreSize)
    {
        float newContentSizeWidth = 0.0f;
        float nextPosY = 0.0f;
        int rowSize = (int)_elementRenders.size();
        for (int i = rowSize - 1; i >= 0; i--)
        {
            float rowWidth = 0.0f;
            float nextPosX = 0.0f;
            float maxY = 0.0f;
            Vector<Node *> *row = _elementRenders.at(i);
            for (auto& iter : *row)
            {
                iter->setAnchorPoint(Vec2::ZERO);
                iter->setPosition(nextPosX, nextPosY);
                this->addProtectedChild(iter, 1);
                Size iSize = iter->getContentSize();
                rowWidth += iSize.width;
                nextPosX += iSize.width;
                maxY = MAX(maxY, iSize.height);
            }
            nextPosY += maxY;
            newContentSizeWidth = MAX(newContentSizeWidth, rowWidth);
        }
        this->setContentSize(Size(newContentSizeWidth, nextPosY));
    }
    else
    {
        float newContentSizeHeight = 0.0f;
        float *maxHeights = new (std::nothrow) float[_elementRenders.size()];
        
        for (size_t i=0, size = _elementRenders.size(); i<size; i++)
        {
            Vector<Node*>* row = (_elementRenders[i]);
            float maxHeight = 0.0f;
            for (auto& iter : *row)
            {
                maxHeight = MAX(iter->getContentSize().height, maxHeight);
            }
            maxHeights[i] = maxHeight;
            newContentSizeHeight += maxHeights[i];
        }
        
        float nextPosY = _customSize.height;
        for (size_t i=0, size = _elementRenders.size(); i<size; i++)
        {
            Vector<Node*>* row = (_elementRenders[i]);
            float nextPosX = 0.0f;
            nextPosY -= (maxHeights[i] + _verticalSpace);
            
            for (auto& iter : *row)
            {
                iter->setAnchorPoint(Vec2::ZERO);
                iter->setPosition(nextPosX, nextPosY);
                this->addProtectedChild(iter, 1);
                nextPosX += iter->getContentSize().width;
            }
        }
        delete [] maxHeights;
    }
    
    for (auto& iter : _elementRenders)
    {
        iter->clear();
        delete iter;
    }
    _elementRenders.clear();
    
    if (_ignoreSize)
    {
        Size s = getVirtualRendererSize();
        this->setContentSize(s);
    }
    else
    {
        this->setContentSize(_customSize);
    }
    updateContentSizeWithTextureSize(_contentSize);
}

void RichText::adaptRenderers()
{
    this->formatText();
}

void RichText::pushToContainer(cocos2d::Node *renderer)
{
    if (_elementRenders.size() <= 0)
    {
        return;
    }
    _elementRenders[_elementRenders.size()-1]->pushBack(renderer);
}

void RichText::setVerticalSpace(float space)
{
    _verticalSpace = space;
}

void RichText::ignoreContentAdaptWithSize(bool ignore)
{
    if (_ignoreSize != ignore)
    {
        _formatTextDirty = true;
        Widget::ignoreContentAdaptWithSize(ignore);
    }
}

std::string RichText::getDescription() const
{
    return "RichText";
}

}

NS_CC_END
