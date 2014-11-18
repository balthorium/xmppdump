#include <iostream>
#include <exception>
#include "xmlpp.h"

using namespace std;

class IndentUnderflowException : public exception
{};

XmlPrettyPrinter::XmlPrettyPrinter() :
    mSlashFound(false)
{}

XmlPrettyPrinter::~XmlPrettyPrinter() 
{}

ostringstream &XmlPrettyPrinter::getPrettyStream()
{
    return mStream;
}

bool XmlPrettyPrinter::parse(const char *xmlbuf, int xmllen)
{
    mXmlIn = xmlbuf;
    mXmlLen = xmllen;
    bool done = false;
    bool retval = false;
    int indent = 0;
    while (!done)
    {
        try
        {
            parse(indent);
            done = true;
            retval = true;
        }
        catch (IndentUnderflowException e)
        {
            indent++;
        }
        catch (...)
        {
            done = true;
            retval = false;
        }
    }
    return retval;
}
    
void XmlPrettyPrinter::parse(int indent)              
{
    mIndentLevel = indent;
    mCur = 0; 
    while (mCur < mXmlLen) 
    {
        if (mXmlIn[mCur] == '<')
        {
            mSlashFound = false;
            appendOpeningBrace();
            bleedWhitespace();
            appendTagName();
            bleedWhitespace();
            int attrCount = 0;
            while ((mCur < mXmlLen) && 
                    (mXmlIn[mCur] != '/') && 
                    (mXmlIn[mCur] != '?') && 
                    (mXmlIn[mCur] != '>'))
            {
                if (attrCount++ > 0)
                {
                    mStream << "\n";
                    appendIndent(mIndentLevel+1);
                }
                appendAttribute();
                bleedWhitespace();
            }
            appendClosingBrace();
            if (!mSlashFound)
            {
                mIndentLevel++;
            }
        }
        else
        {
            appendCDATA();
        }
    }
}
        
void XmlPrettyPrinter::bleedWhitespace()
{
    while (mCur < mXmlLen && isspace(mXmlIn[mCur]))
    {
        ++mCur;
    }
}
    
void XmlPrettyPrinter::appendOpeningBrace() 
{
    if (mCur < mXmlLen)
    {
        if ((mCur+1 < mXmlLen) && 
                (mXmlIn[mCur] == '<') && 
                (mXmlIn[mCur+1] == '/'))
        {   
            mIndentLevel--;
            if (mIndentLevel < 0)
            {
                throw IndentUnderflowException();
            }
            appendIndent(mIndentLevel);
            mStream << "</";
            mSlashFound = true;
            mCur+=2;
        }
        else if ((mCur+1 < mXmlLen) && 
                (mXmlIn[mCur] == '<') && 
                (mXmlIn[mCur+1] == '?'))
        {
            mStream << "<?";
            mSlashFound = true;
            mCur+=2;
        }
        else if (mXmlIn[mCur] == '<')
        {
            appendIndent(mIndentLevel);
            mStream << "<";
            ++mCur;
        }
    }
}

void XmlPrettyPrinter::appendIndent(int level)
{
    for (int i = 0; i < level; ++i)
    {
        mStream << "    ";
    }
}
    
void XmlPrettyPrinter::appendClosingBrace()
{
    if (mCur < mXmlLen)
    {
        if ((mCur+1 < mXmlLen) && (mXmlIn[mCur] == '/') && 
                (mXmlIn[mCur+1] == '>'))
        {   
            mStream << "/>\n";
            mSlashFound = true;
            mCur+=2;
        }
        else if ((mCur+1 < mXmlLen) && (mXmlIn[mCur] == '?') && 
            (mXmlIn[mCur+1] == '>'))
        {   
            mStream << "?>\n";
            mCur+=2;
        }
        else if (mXmlIn[mCur] == '>')
        {   
            mStream << ">\n";
            ++mCur;
        }
    }
}
    
void XmlPrettyPrinter::appendTagName()
{
    while (mCur < mXmlLen)
    {
        if (isspace(mXmlIn[mCur]) || mXmlIn[mCur] == '>' || mXmlIn[mCur] == '/')
        {
            break;
        }
        mStream << mXmlIn[mCur++];
    }
}
    
void XmlPrettyPrinter::appendAttribute()
{
    appendAttributeName();
    bleedWhitespace();
    appendEquals();
    bleedWhitespace();
    appendQuote();
    appendAttributeValue();
    appendQuote();
}

void XmlPrettyPrinter::appendAttributeName()
{
    mStream << " ";
    while (mCur < mXmlLen)
    {
        if (isspace(mXmlIn[mCur]) || (mXmlIn[mCur] == '='))
        {
            break;
        }
        mStream << mXmlIn[mCur++];
    }
}

void XmlPrettyPrinter::appendEquals()
{
    if ((mCur < mXmlLen) && (mXmlIn[mCur] == '='))
    {   
        mStream << "=";
        ++mCur;
    }
}

void XmlPrettyPrinter::appendQuote()
{
    if ((mCur < mXmlLen) && ((mXmlIn[mCur] == '\'') || (mXmlIn[mCur] == '\"')))
    {   
        mStream << mXmlIn[mCur++];
    }
}

void XmlPrettyPrinter::appendAttributeValue()
{
    while (mCur < mXmlLen)
    {
        if ((mXmlIn[mCur] == '\'') || (mXmlIn[mCur] == '\"'))
        {
            break;
        }
        mStream << mXmlIn[mCur++];
    }
}
    
void XmlPrettyPrinter::appendCDATA()
{
    appendIndent(mIndentLevel);
    while (mCur < mXmlLen)
    {
        if (mXmlIn[mCur] == '<')
        {
            break;
        }
        mStream << mXmlIn[mCur];
        if (mXmlIn[mCur] == '\n' || mXmlIn[mCur] == '\r')
        {
            appendIndent(mIndentLevel);             
        }
        mCur++;
    }
    mStream << "\n";
}   
    
