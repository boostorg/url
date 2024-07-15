/*
    Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    Official repository: https://github.com/boostorg/site-docs
*/

const path = require('path');
const fs = require('fs');
const xpath = require('xpath');
const {XMLParser} = require("fast-xml-parser");
const he = require('he');

/*
    Load tag files

    Default tag files come from the cpp_tags directory.
    It already includes the tags for cppreference.com.

    We still need to implement other strategies and sources for tag files.
    This will require this to become an antora extension.

    To generate the most recent cppreference tags, you can get them from
    https://en.cppreference.com/w/Cppreference:Archives or generate a
    more recent version using the following commands:

    ```
    git clone https://github.com/PeterFeicht/cppreference-doc
    cd cppreference-doc
    make source
    make doc_doxygen
    ```

    The result will be in ./output.
 */
let tagDocs = {}
// Read all files in __dirname
const defaultTagsDir = path.join(__dirname, 'cpp_tags');
const parser = new XMLParser({
    ignoreDeclaration: true,
    ignoreAttributes: false,
    attributesGroupName: "@attributes",
    attributeNamePrefix: "",
    allowBooleanAttributes: true
});
fs.readdirSync(defaultTagsDir).forEach(file => {
    if (file.endsWith('.tag.xml')) {
        const tagFilePath = path.join(defaultTagsDir, file);
        const xml = fs.readFileSync(tagFilePath, 'utf8');
        let tagFileKey = file.replace('.tag.xml', '');
        tagDocs[tagFileKey] = parser.parse(xml);
    }
})

function getFileFilename(doc, symbolName) {
    const targetFilename = symbolName.substring(1, symbolName.length - 1)
    const tagFileObj = doc['tagfile']
    for (const compound of tagFileObj['compound']) {
        if (compound['name'] === targetFilename) {
            return compound['filename']

        }
    }
    return undefined
}

function getSymbolFilename(obj, symbolName, curNamespace) {
    // Iterate <compound kind="class">
    if ('compound' in obj) {
        for (const compound of obj['compound']) {
            const kind = compound['@attributes']["kind"];
            if (kind !== 'class') {
                continue
            }
            const name = compound['name'];
            if (symbolName === name && 'filename' in compound) {
                return compound['filename']
            }
        }
    }

    // Iterate <member kind="function">
    if ('member' in obj) {
        for (const member of obj['member']) {
            const kind = member['@attributes']["kind"];
            if (kind !== 'function') {
                continue
            }
            const name = member['name']
            const qualifiedName = curNamespace ? curNamespace + "::" + name : name
            if (symbolName === qualifiedName && 'anchorfile' in member) {
                return member['anchorfile'] + (('anchor' in member && member['anchor']) ? `#${member["anchor"]}` : '')
            }
        }
    }

    // <compound kind="namespace">
    if ('compound' in obj) {
        for (const compound of obj['compound']) {
            const kind = compound['@attributes']["kind"];
            if (kind !== 'namespace' && kind !== 'class') {
                continue
            }
            const name = compound['name'];
            if (symbolName === name && 'filename' in compound) {
                return compound['filename']
            }
            if (symbolName.startsWith(name + '::')) {
                const res = getSymbolFilename(compound, symbolName, name)
                if (res) {
                    return res
                }
            }
        }
    }

    return undefined
}

function splitCppSymbol(symbolName) {
    let mainSymbolName = ''
    let curTemplate = ''
    let templateParameters = []
    let rest = ''
    let level = 0
    let inTemplateParameters = false

    for (let i = 0; i < symbolName.length; i++) {
        let c = symbolName[i];
        if (c === '<') {
            inTemplateParameters = true
            level++;
        } else if (c === '>') {
            level--;
        }

        if (level === 0) {
            if (!inTemplateParameters) {
                mainSymbolName += c
            } else {
                rest = symbolName.substring(i + 1)
                break
            }
        } else if (level === 1 && [',', '>', '<'].includes(c)) {
            if (curTemplate) {
                if (c === '>') {
                    curTemplate += c
                }
                templateParameters.push(curTemplate.trim())
                curTemplate = ''
            }
        } else {
            curTemplate += c
        }
    }

    if (curTemplate) {
        templateParameters.push(curTemplate.trim())
    }

    return {
        mainSymbolName: mainSymbolName,
        templateParameters: templateParameters,
        rest: rest
    };
}


/**
 * Gets the URL for a symbol.
 *
 * @param doc - The XML document containing the symbols.
 * @param symbolName - The name of the symbol.
 * @returns {undefined|string} The URL for the symbol, or undefined if the symbol is not found.
 */
function getSymbolLink(doc, symbolName) {
    if (isFundamentalType(symbolName)) {
        return `https://en.cppreference.com/w/cpp/language/types[${symbolName},window=_blank]`
    }

    const isIncludeFile = symbolName.startsWith('"') && symbolName.endsWith('"') ||
        symbolName.startsWith('<') && symbolName.endsWith('>');
    if (isIncludeFile) {
        const filename = getFileFilename(doc, symbolName)
        if (filename) {
            return `https://en.cppreference.com/w/${filename}[${he.encode(symbolName)},window="_blank"]`
        }
        return undefined
    }

    const tagFileObj = doc['tagfile']
    const isTemplate = symbolName.includes('<')
    if (!isTemplate) {
        const filename = getSymbolFilename(tagFileObj, symbolName, '')
        if (filename !== undefined) {
            return `https://en.cppreference.com/w/${filename}[${he.encode(symbolName)},window="_blank"]`
        }
    } else {
        const {mainSymbolName, templateParameters, rest} = splitCppSymbol(symbolName)
        let fullLink = ''
        const mainLink = getSymbolLink(doc, mainSymbolName)
        if (mainLink) {
            fullLink = mainLink
        } else {
            fullLink = he.encode(mainSymbolName)
        }
        fullLink += he.encode('<')
        let isFirst = true
        for (const templateParameter of templateParameters) {
            const templateParameterLink = getSymbolLink(doc, templateParameter)
            if (!isFirst) {
                fullLink += he.encode(', ')
            }
            if (templateParameterLink) {
                fullLink += templateParameterLink
            } else {
                fullLink += he.encode(templateParameter)
            }
            isFirst = false
        }
        fullLink += he.encode('>')
        if (rest.startsWith('::')) {
            fullLink += he.encode('::')
            const restLink = getSymbolLink(doc, mainSymbolName + rest)
            if (restLink) {
                fullLink += restLink
            } else {
                fullLink += he.encode(rest.substring(2))
            }
        } else {
            fullLink += he.encode(rest)
        }
        return fullLink
    }
    return undefined
}

/**
 * Returns true if the given type is a fundamental type.
 * This is used to link to the cppreference.com documentation for the type.
 *
 * @see https://en.cppreference.com/w/cpp/language/types
 *
 * @param type - The type to check.
 * @returns {boolean} True if the type is a fundamental type.
 */
function isFundamentalType(type) {
    return ['bool', 'char', 'char8_t', 'char16_t', 'char32_t', 'wchar_t', 'int', 'signed', 'unsigned', 'short', 'long', 'float', 'true', 'false', 'double', 'void'].includes(type)
}

/**
 * Registers an inline macro named "cpp" with the given Asciidoctor registry.
 * This macro creates a link to the corresponding Boost C++ symbol using the
 * provided Doxygen tag files.
 *
 * @param {Object} registry - The Asciidoctor registry to register the macro with.
 * @throws {Error} If registry is not defined.
 * @example
 * const asciidoctor = require('asciidoctor');
 * const registry = asciidoctor.Extensions.create();
 * registerBoostMacro(registry);
 */
module.exports = function (registry) {
    // Make sure registry is defined
    if (!registry) {
        throw new Error('registry must be defined');
    }

    // Try to convert this into an antora extension so that it can receive
    // parameters just like mermaid can receive parameters:
    // https://github.com/snt/antora-mermaid-extension/blob/master/lib/extension.js
    //
    // From these parameters, we can already determine other tag files to load
    // according to the component.
    // Here's how the components set these tagfiles:
    // https://gitlab.com/antora/antora-collector-extension/-/tree/main/packages/collector-extension?ref_type=heads

    /**
     * Processes the "cpp" inline macro.
     * If the "title" attribute is specified, it is used as the link text.
     * Otherwise, the monospace code target is used as the link text.
     * The link URL is constructed based on the snake_case version of the target.
     *
     * @param {Object} parent - The parent node of the inline macro.
     * @param {string} target - The target of the inline macro.
     * @param {Object} attr - The attributes of the inline macro.
     * @returns {Object} An inline node representing the link.
     */
    registry.inlineMacro('cpp', function () {
        const self = this;
        self.process(function (parent, target, attr) {
            // Decode HTML
            target = he.decode(target)
            // const DEFAULT_BOOST_BRANCH = 'master'
            // const branch = parent.getDocument().getAttribute('page-boost-branch', DEFAULT_BOOST_BRANCH)
            for (const [tag, doc] of Object.entries(tagDocs)) {
                const link = getSymbolLink(doc, target)
                if (link) {
                    return self.createInline(parent, 'quoted', link, {type: 'monospaced'})
                }
            }
            // If the symbol is not found, return the target as monospaced text
            return self.createInline(parent, 'quoted', target, {type: 'monospaced'})
        })
    })
}

if (require.main === module) {
    console.log('Tests')
    for (const [tag, doc] of Object.entries(tagDocs)) {
        console.log(getSymbolLink(doc, 'std'))
        console.log(getSymbolLink(doc, 'bool'))
        console.log(getSymbolLink(doc, 'std::string_view'))
        console.log(getSymbolLink(doc, 'std::string_view'))
        console.log(getSymbolLink(doc, 'std::FILE'))
        console.log(getSymbolLink(doc, 'std::vector'))
        console.log(getSymbolLink(doc, 'std::vector<int>'))
        console.log(getSymbolLink(doc, 'std::vector<std::vector<int>,std::allocator<int>>'))
        console.log(getSymbolLink(doc, 'MyClass<int,std::vector<double>,std::map<int,std::string>>'))
        console.log(getSymbolLink(doc, 'std::vector<std::vector<int>,std::allocator<int>>::const_iterator'))
        console.log(getSymbolLink(doc, '"algorithm"'))
        console.log(getSymbolLink(doc, '<algorithm>'))
        console.log(getSymbolLink(doc, 'std::abort'))
        console.log(getSymbolLink(doc, 'std::allocator::address'))
    }
}
