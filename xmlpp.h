/**
 * (c) Copyright 2015 Andrew Biggs
 * This code is available under the Apache License, version 2: http://www.apache.org/licenses/LICENSE-2.0.html
 */

#include <sstream>

class XmlPrettyPrinter
{
public:
    XmlPrettyPrinter();
    virtual ~XmlPrettyPrinter();

    bool parse(const char *xml, int len);
    std::ostringstream &getPrettyStream();

private:
    XmlPrettyPrinter(const XmlPrettyPrinter &);
    XmlPrettyPrinter &operator=(const XmlPrettyPrinter &);

    void parse(int indent);
    void bleedWhitespace();
    void appendOpeningBrace();
    void appendIndent(int level);
    void appendClosingBrace();
    void appendTagName();
    void appendAttribute();
    void appendAttributeName();
    void appendEquals();
    void appendQuote();
    void appendAttributeValue();
    void appendCDATA();

    const char *mXmlIn;
    int mXmlLen;
    int mCur;
    std::ostringstream mStream;   
    bool mSlashFound;
    int mIndentLevel;
};

