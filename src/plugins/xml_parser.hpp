#include <iostream>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlschemas.h>

/**
 * @brief XSD生成器类
 */
class XsdGenerator {
public:
    /**
     * @brief 生成XSD文件
     * @param filename XSD文件名
     * @return true：成功；false：失败
     */
    bool GenerateXsd(const char* filename);

private:
    /**
     * @brief 创建XML元素类型节点
     * @param doc xmlDocPtr对象
     * @param typeName 类型名称
     * @param baseTypeName 基本类型名称
     * @return xmlNodePtr对象
     */
    xmlNodePtr CreateTypeNode(xmlDocPtr doc, const char* typeName, const char* baseTypeName);

    /**
     * @brief 创建XML元素节点
     * @param doc xmlDocPtr对象
     * @param elementName 元素名称
     * @param typeName 类型名称
     * @return xmlNodePtr对象
     */
    xmlNodePtr CreateElementNode(xmlDocPtr doc, const char* elementName, const char* typeName);
};

bool XsdGenerator::GenerateXsd(const char* filename) {
    spdlog::info("开始生成XSD文件：{}", filename);

    xmlDocPtr xsd_doc = xmlNewDoc(BAD_CAST "1.0");
    if (xsd_doc == NULL) {
        spdlog::error("创建XSD文档失败");
        return false;
    }

    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "xs:schema");
    xmlNewProp(root, BAD_CAST "xmlns:xs", BAD_CAST "http://www.w3.org/2001/XMLSchema");
    xmlDocSetRootElement(xsd_doc, root);

    xmlNodePtr type_node = this->CreateTypeNode(xsd_doc, "ElementType", "xs:string");
    xmlAddChild(root, type_node);

    xmlNodePtr element_node = this->CreateElementNode(xsd_doc, "Element", "ElementType");
    xmlAddChild(root, element_node);

    if (xmlSchemaDump(filename, xsd_doc) < 0) {
        spdlog::error("生成XSD文件失败：{}", filename);
        xmlFreeDoc(xsd_doc);
        return false;
    }

    xmlFreeDoc(xsd_doc);
    spdlog::info("成功生成XSD文件：{}", filename);
    return true;
}

xmlNodePtr XsdGenerator::CreateTypeNode(xmlDocPtr doc, const char* typeName, const char* baseTypeName) {
    xmlNodePtr type_node = xmlNewNode(NULL, BAD_CAST "xs:simpleType");
    xmlNewProp(type_node, BAD_CAST "name", BAD_CAST typeName);

    xmlNodePtr restriction_node = xmlNewChild(type_node, NULL, BAD_CAST "xs:restriction", NULL);
    xmlNewProp(restriction_node, BAD_CAST "base", BAD_CAST baseTypeName);

    return type_node;
}

xmlNodePtr XsdGenerator::CreateElementNode(xmlDocPtr doc, const char* elementName, const char* typeName) {
    xmlNodePtr element_node = xmlNewNode(NULL, BAD_CAST "xs:element");
    xmlNewProp(element_node, BAD_CAST "name", BAD_CAST elementName);
    xmlNewProp(element_node, BAD_CAST "type", BAD_CAST typeName);

    return element_node;
}

/**
 * @brief XML处理器类
 */
class XmlHandler {
public:
    /**
     * @brief 加载XML文件
     * @param filename 文件名
     * @return true：成功；false：失败
     */
    bool LoadXml(const char* filename);

    /**
     * @brief 保存XML文件
     * @param filename 文件名
     * @return true：成功；false：失败
     */
    bool SaveXml(const char* filename);

    /**
     * @brief 打印所有元素名称和值
     */
    void PrintElements();

    /**
     * @brief 修改指定元素的值
     * @param elementName 元素名称
     * @param newValue 新值
     */
    void ModifyElementValue(const char* elementName, const char* newValue);

private:
    xmlDocPtr doc;
};

bool XmlHandler::LoadXml(const char* filename) {
    spdlog::info("开始加载XML文件：{}", filename);

    this->doc = xmlReadFile(filename, NULL, 0);
    
    if (this->doc == NULL) {
        spdlog::error("加载XML文件失败：{}", filename);
        return false;
    }

    spdlog::info("成功加载XML文件：{}", filename);
    return true;
}

bool XmlHandler::SaveXml(const char* filename) {
    spdlog::info("开始保存XML文件：{}", filename);

    if (xmlSaveFormatFileEnc(filename, this->doc, "UTF-8", 1) < 0) {
        spdlog::error("保存XML文件失败：{}", filename);
        return false;
    }

    spdlog::info("成功保存XML文件：{}", filename);
    return true;
}

void XmlHandler::PrintElements() {
    xmlNodePtr root = xmlDocGetRootElement(this->doc);
    if (root == NULL) {
        std::cout << "Failed to find root element." << std::endl;
        return;
    }

    for (xmlNodePtr node = root->children; node != NULL; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            std::cout << "Name: " << node->name << ", Value: " << xmlNodeGetContent(node) << std::endl;
        }
    }
}

void XmlHandler::ModifyElementValue(const char* elementName, const char* newValue) {
    xmlNodePtr root = xmlDocGetRootElement(this->doc);
    if (root == NULL) {
        std::cout << "Failed to find root element." << std::endl;
        return;
    }

    for (xmlNodePtr node = root->children; node != NULL; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, BAD_CAST elementName) == 0) {
            xmlChar* content = xmlNodeGetContent(node);
            xmlNodeSetContent(node, BAD_CAST newValue);
            xmlFree(content);
            break;
        }
    }

    spdlog::info("修改XML元素值：{} -> {}", elementName, newValue);
}

int main() {

    // 初始化spdlog日志记录器
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    auto logger = spdlog::stdout_color_mt("XsdGenerator");

    XsdGenerator generator;

    if (!generator.GenerateXsd("example.xsd")) {
        return 1;
    }

    logger->info("程序执行完成");
    
    // 清理 Libxml2 

    // 初始化spdlog日志记录器
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    auto logger = spdlog::stdout_color_mt("XmlHandler");

    XmlHandler handler;

    if (!handler.LoadXml("example.xml")) {
        return 1;
    }

    handler.PrintElements();
    handler.ModifyElementValue("element2", "New Value");

    if (!handler.SaveXml("modified.xml")) {
        return 1;
    }

    logger->info("程序执行完成");

    // 清理 Libxml2 库
    xmlFreeDoc(handler.doc);
    xmlCleanupParser();

    return 0;
}