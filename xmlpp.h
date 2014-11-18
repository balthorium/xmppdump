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

