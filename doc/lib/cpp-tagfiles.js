/*
    Copyright (c) 2024 Alan de Freitas (alandefreitas@gmail.com)

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    Official repository: https://github.com/boostorg/site-docs
*/

'use strict'

const path = require('path');
const he = require("he");
const {XMLParser} = require("fast-xml-parser");
const fs = require("fs");
const assert = require("assert");

/**
 * CppTagfilesExtension is a class that handles the registration and processing of C++ tagfiles.
 * It is used to extend the functionality of the Antora documentation generator with the
 * inline macro `cpp:<symbol-name>[]` which transforms the symbol name into a link to the
 * cppreference.com documentation or the documentation in any doxygen tagfile.
 *
 * The extra tagfiles can be registered in
 * - the playbook file (antora.extensions.<cpp-tagfile-entry>,files) or
 * - the component descriptor file (ext.cpp-tagfiles.files)
 *
 * Each entry in `files` should contain the path to the tagfile and
 * the base URL for the documentation.
 *
 * The class registers itself to the generator context and listens to two events:
 * - 'contentAggregated': When this event is triggered, the class reads the tagfiles and parses them.
 * - 'beforeProcess': When this event is triggered, the class registers an AsciiDoc processor that processes 'cpp' inline macros.
 *
 * See https://docs.antora.org/antora/latest/extend/class-based-extension/
 *
 * The class also provides methods to get the URL for a symbol and to check if a given type is a fundamental type.
 *
 * @class
 * @property {Object} context - The generator context.
 * @property {Array} tagfiles - An array of tagfile objects.
 * @property {Object} logger - The logger object.
 * @property {Object} config - The configuration object.
 * @property {Object} playbook - The playbook object.
 *
 */
class CppTagfilesExtension {
    static register({config, playbook}) {
        new CppTagfilesExtension(this, {config, playbook})
    }

    constructor(generatorContext, {config, playbook}) {
        ;(this.context = generatorContext)
            .on('contentAggregated', this.onContentAggregated.bind(this))
            .on('beforeProcess', this.onBeforeProcess.bind(this))

        this.tagfiles = []
        this.usingNamespaces = []
        this.cache = {}

        // https://www.npmjs.com/package/@antora/logger
        // https://github.com/pinojs/pino/blob/main/docs/api.md
        this.logger = this.context.getLogger('cpp-tagfile-extension')
        this.logger.info('Registering cpp-tagfile-extension')

        this.config = config
        // playbook = playbook
    }

    /**
     * Checks if a cached result exists for a given symbol and component.
     *
     * The cache is a map of maps. The first map is the symbol. The second level map is the component.
     * The value in the second map is the link.
     *
     * The reason we have two levels is that components might have different tagfiles. Thus,
     * the same target can have different links depending on the component.
     *
     * @param {string} symbol - The symbol for which the cached result is to be checked.
     * @param {string} component - The component for which the cached result is to be checked.
     * @returns {boolean} True if a cached result exists for the given symbol and component, false otherwise.
     */
    hasCachedResult(symbol, component) {
        return symbol in this.cache && component in this.cache[symbol]
    }

    /**
     * Retrieves a cached result for a given symbol and component.
     *
     * @param {string} symbol - The symbol for which the cached result is to be retrieved.
     * @param {string} component - The component for which the cached result is to be retrieved.
     * @returns {string} The cached result for the given symbol and component.
     */
    getCachedResult(symbol, component) {
        if (this.hasCachedResult(symbol, component)) {
            return this.cache[symbol][component]
        }
        return undefined
    }

    /**
     * Sets a cached result for a given symbol and component.
     *
     * If no cache exists for the given symbol, a new cache is created.
     *
     * @param {string} symbol - The symbol for which the cached result is to be set.
     * @param {string} component - The component for which the cached result is to be set.
     * @param {string} result - The result to be cached.
     */
    setCachedResult(symbol, component, result) {
        if (!(symbol in this.cache)) {
            this.cache[symbol] = {}
        }
        this.cache[symbol][component] = result
    }

    // https://docs.antora.org/antora/latest/extend/add-event-listeners/
    // https://docs.antora.org/antora/latest/extend/generator-events-reference/

    /**
     * Event handler for the 'contentAggregated' event.
     * This event is triggered after all the content sources have been cloned and
     * the aggregate of all the content has been created.
     *
     * This method reads the tagfiles and parses them. The tagfiles are registered
     * in the playbook file or the component descriptor files.
     *
     * The method also removes duplicate tagfiles and logs the final list of tagfiles.
     *
     * @param {Object} playbook - The playbook object.
     * @param {Object} siteAsciiDocConfig - The AsciiDoc configuration for the site.
     * @param {Object} siteCatalog - The site catalog object.
     * @param {Array} contentAggregate - The aggregate of all the content.
     */
    onContentAggregated({playbook, siteAsciiDocConfig, siteCatalog, contentAggregate}) {
        this.logger.info('Reading tagfiles')

        function externalAsBoolean(tagfile) {
            const baseUrl = tagfile.baseUrl ? tagfile.baseUrl : '';
            const baseUrlIsHttp = baseUrl.startsWith('http://') || baseUrl.startsWith('https://')
            const externalIsBoolean = typeof tagfile.external === 'boolean'
            const externalIsString = typeof tagfile.external === 'string'
            return externalIsBoolean ?
                tagfile.external :
                externalIsString ?
                    tagfile.external === 'true' :
                    baseUrlIsHttp
        }

        // Iterate tagfiles set in the components
        for (const componentVersionBucket of contentAggregate.slice()) {
            const {origins = []} = componentVersionBucket
            for (const origin of origins) {
                const {descriptor, worktree} = origin
                let tagFilesConfig = descriptor?.ext?.cppTagfiles?.files || []
                for (const tagfile of tagFilesConfig) {
                    this.tagfiles.push({
                        file: path.join(worktree, tagfile.file),
                        baseUrl: tagfile.baseUrl ? tagfile.baseUrl : '',
                        component: descriptor.name,
                        external: externalAsBoolean(tagfile)
                    })
                }
            }
        }

        // Iterate tagfiles in the playbook
        const playbookFiles = this.config?.cppTagfiles?.files || []
        playbookFiles.forEach(tagfile => {
            const baseUrl = tagfile.baseUrl ? tagfile.baseUrl : '';
            this.tagfiles.push({
                file: path.join(playbook.dir, tagfile.file),
                baseUrl: baseUrl,
                component: null,
                external: externalAsBoolean(tagfile.external)
            })
        })

        // Add the cppreference tagfile
        //
        // To generate the most recent cppreference tags, you can get them from
        // https://en.cppreference.com/w/Cppreference:Archives or generate a
        // more recent version using the following commands:
        //
        // ```
        // git clone https://github.com/PeterFeicht/cppreference-doc
        // cd cppreference-doc
        // make source
        // make doc_doxygen
        // ```
        //
        // The result will be in ./output.
        const defaultTagfilesDir = path.join(__dirname, 'cpp_tagfiles');
        this.tagfiles.push({
            file: path.join(defaultTagfilesDir, 'cppreference-doxygen-web.tag.xml'),
            baseUrl: 'https://en.cppreference.com/w/',
            component: null,
            external: true
        })

        // Remove duplicates considering both the file and the component
        this.tagfiles = this.tagfiles.filter((tagfile, index, self) =>
            index === self.findIndex((t) => (
                t.file === tagfile.file && t.component === tagfile.component
            )))
        this.logger.info(this.tagfiles, 'tagfiles')

        // Read the tagfiles
        const parser = new XMLParser({
            ignoreDeclaration: true,
            ignoreAttributes: false,
            attributesGroupName: "@attributes",
            attributeNamePrefix: "",
            allowBooleanAttributes: true
        });
        this.tagfiles.forEach(tagfile => {
            const {file} = tagfile
            if (!fs.existsSync(file)) {
                this.logger.error(`Tagfile not found: ${file}`)
                return
            }
            const xml = fs.readFileSync(file, 'utf8');
            tagfile.doc = parser.parse(xml);
        })

        // Iterate using-namespace set in the components
        for (const componentVersionBucket of contentAggregate.slice()) {
            const {origins = []} = componentVersionBucket
            for (const origin of origins) {
                const {descriptor, worktree} = origin
                let usingNamespacesConfig = descriptor?.ext?.cppTagfiles?.usingNamespaces || []
                for (const namespace of usingNamespacesConfig) {
                    this.usingNamespaces.push({
                        namespace: namespace.endsWith('::') ? namespace : namespace + '::',
                        component: descriptor.name
                    })
                }
            }
        }

        // Iterate tagfiles in the playbook
        const playbookNamespaces = this.config?.cppTagfiles?.usingNamespaces || []
        playbookNamespaces.forEach(namespace => {
            this.usingNamespaces.push({
                namespace: namespace.endsWith('::') ? namespace : namespace + '::',
                component: null
            })
        })

        // Add the default using namespace
        this.usingNamespaces.push({
            namespace: 'std::',
            component: null
        })

        // Remove duplicates considering both the file and the component
        this.usingNamespaces = this.usingNamespaces.filter((tagfile, index, self) =>
            index === self.findIndex((t) => (
                t.namespace === tagfile.namespace && t.component === tagfile.component
            )))
    }

    /**
     * Event handler for the 'beforeProcess' event.
     * This event is triggered before the content is processed.
     *
     * This method registers an AsciiDoc processor that processes 'cpp' inline macros.
     * The processor transforms the symbol name into a link to the cppreference.com
     * documentation or the documentation in any doxygen tagfile.
     *
     * @param {Object} playbook - The playbook object.
     * @param {Object} siteAsciiDocConfig - The AsciiDoc configuration for the site.
     * @param {Object} siteCatalog - The site catalog object.
     */
    onBeforeProcess({playbook, siteAsciiDocConfig, siteCatalog}) {
        this.logger.info('Registering the tagfiles asciidoc processor')
        if (!siteAsciiDocConfig.extensions) siteAsciiDocConfig.extensions = []
        const extensionSelf = this
        siteAsciiDocConfig.extensions.push({
            register: (registry, _context) => {
                registry.inlineMacro('cpp', function () {
                    const self = this;
                    self.process(function (parent, target, attr) {
                        const linkText = '$positional' in attr ? attr['$positional'][0] : undefined
                        const component = parent.document.getAttributes()['page-component-name'] || null
                        const decodedSymbol = he.decode(target);
                        const fromCache = linkText ? false :
                            extensionSelf.hasCachedResult(decodedSymbol, component)
                        let link =
                            fromCache ?
                                extensionSelf.getCachedResult(decodedSymbol, component) :
                                extensionSelf.getSymbolLink(decodedSymbol, linkText, component, '')
                        if (!fromCache && !linkText) {
                            extensionSelf.setCachedResult(decodedSymbol, component, link)
                        }
                        if (link) {
                            return self.createInline(parent, 'quoted', link, {type: 'monospaced'})
                        }
                        return self.createInline(parent, 'quoted', linkText ? linkText : target, {type: 'monospaced'})
                    })
                })
                return registry
            }
        })
    }

    /**
     * Gets the URL for a symbol.
     *
     * @param symbol - The name of the symbol.
     * @param component - The Antora component the symbol belongs to.
     * @param namespace - The namespace where we will look for the symbol
     * @returns {undefined|string} The URL for the symbol, or undefined if the symbol is not found.
     */
    getSymbolLink(symbol, linkText, component, namespace) {
        if (CppTagfilesExtension.isFundamentalType(symbol)) {
            return `https://en.cppreference.com/w/cpp/language/types${CppTagfilesExtension.getFundamentalTypeAnchor(symbol)}[${symbol},window=_blank]`
        }

        // Handle links to include files
        const isIncludeFile = symbol.startsWith('"') && symbol.endsWith('"') ||
            symbol.startsWith('<') && symbol.endsWith('>');
        if (isIncludeFile) {
            for (const tagfile of this.tagfiles) {
                if (tagfile.component !== null && tagfile.component !== component) {
                    continue
                }
                const filename = CppTagfilesExtension.getFileFilename(tagfile.doc, symbol)
                if (filename) {
                    return `${tagfile.baseUrl}${filename}[${linkText ? linkText : he.encode(symbol)}${tagfile.external ? ',window="_blank"' : ''}]`
                }
            }
            return undefined
        }

        // Handle symbols
        const isTemplate = symbol.includes('<');
        if (!isTemplate) {
            // Handle symbols that are not templates
            for (const tagfile of this.tagfiles) {
                if (tagfile.component !== null && tagfile.component !== component) {
                    continue
                }
                const qualifiedSymbol = namespace + symbol
                this.logger.trace(`Looking for ${symbol} in namespace ${namespace} in ${tagfile.file}`)
                const filename = CppTagfilesExtension.getSymbolFilename(tagfile.doc['tagfile'], qualifiedSymbol, '')
                if (filename !== undefined) {
                    return `${tagfile.baseUrl}${filename}[${linkText ? linkText : he.encode(symbol)}${tagfile.external ? ',window="_blank"' : ''}]`
                }
            }

            const lookInNamespaces = namespace === ''
            if (lookInNamespaces) {
                for (const usingNamespace of this.usingNamespaces) {
                    if (usingNamespace.component !== null && usingNamespace.component !== component) {
                        continue
                    }
                    assert(usingNamespace.namespace.endsWith('::'), 'Namespace should end with ::')
                    const result = this.getSymbolLink(symbol, linkText, component, usingNamespace.namespace)
                    if (result !== undefined) {
                        return result
                    }
                }
            }
        } else {
            // Handle symbols that are templates
            const {mainSymbolName, templateParameters, rest} = CppTagfilesExtension.splitCppSymbol(symbol)
            let fullLink
            const mainLink = this.getSymbolLink(mainSymbolName, linkText, component, namespace)
            if (mainLink) {
                fullLink = mainLink
            } else {
                fullLink = he.encode(mainSymbolName)
            }
            if (linkText) {
                // If the link text is provided, we return a single link to the main symbol
                return fullLink
            }
            fullLink += he.encode('<')
            let isFirst = true
            for (const templateParameter of templateParameters) {
                const templateParameterLink = this.getSymbolLink(templateParameter, linkText, component, namespace)
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
                const restLink = this.getSymbolLink(mainSymbolName + rest, linkText, component, namespace)
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
     * Returns the anchor for a given fundamental type symbol.
     *
     * This function checks if the provided symbol is a fundamental type. If it is, the function returns the anchor for the type.
     * The anchors are used to link to the cppreference.com documentation for the type.
     *
     * @see https://en.cppreference.com/w/cpp/language/types
     *
     * @param {string} symbol - The fundamental type symbol.
     * @returns {string} The anchor for the fundamental type symbol, or an empty string if the symbol is not a fundamental type.
     */
    static getFundamentalTypeAnchor(symbol) {
        const anchors = {
            'bool': '#Boolean_type',
            'char': '#Character_types',
            'char8_t': '#Character_types',
            'char16_t': '#Character_types',
            'char32_t': '#Character_types',
            'wchar_t': '#Character_types',
            'int': '#Standard_integer_types',
            'signed': '#Standard_integer_types',
            'unsigned': '#Standard_integer_types',
            'short': '#Standard_integer_types',
            'long': '#Standard_integer_types',
            'float': '#Floating-point_types',
            'true': '#Boolean_type',
            'false': '#Boolean_type',
            'double': '#Floating-point_types',
            'long double': '#Floating-point_types',
            'void': '#void',
        }
        if (symbol in anchors) {
            return anchors[symbol]
        }
        return ''
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
    static isFundamentalType(type) {
        return ['bool', 'char', 'char8_t', 'char16_t', 'char32_t', 'wchar_t', 'int', 'signed', 'unsigned', 'short', 'long', 'float', 'true', 'false', 'double', 'long double', 'void'].includes(type)
    }

    /**
     * Retrieves the filename URL of a given file from the provided documentation object.
     *
     * For instance, it returns 'cpp/header/algorithm' for the symbol 'algorithm',
     * which is the filename URL for the cppreference.com documentation.
     *
     * This function iterates over the 'compound' elements in the documentation object,
     * looking for a match with the provided symbol. If a match is found, the filename
     * associated with that compound is returned.
     *
     * @param {Object} doc - The documentation object parsed from a tagfile.
     * @param {string} symbol - The symbol for which the filename is to be retrieved.
     * @returns {string|undefined} The filename associated with the symbol, or undefined if the symbol is not found.
     */
    static getFileFilename(doc, symbol) {
        const targetFilename = symbol.substring(1, symbol.length - 1)
        assert('tagfile' in doc, 'tagfile should be in doc')
        const tagFileObj = doc['tagfile']
        if ('compound' in tagFileObj) {
            for (const compound of tagFileObj['compound']) {
                if (compound['name'] === targetFilename) {
                    return compound['filename']

                }
            }
        }
        return undefined
    }

    /**
     * Retrieves the filename URL of a given symbol from the provided documentation object.
     *
     * This function iterates over the 'compound' elements in the documentation object,
     * looking for a match with the provided symbol. If a match is found, the filename
     * associated with that compound is returned.
     *
     * For instance, it returns 'cpp/container/vector' for the symbol 'std::vector'.
     *
     * The function also handles namespaces and classes by checking if the symbol
     * starts with the name of the namespace or class.
     * If it does, it recursively calls itself with the current namespace or class
     * as the new object and the remaining part of the symbol.
     *
     * @param {Object} obj - The documentation object parsed from a tagfile.
     * @param {string} symbol - The symbol for which the filename is to be retrieved.
     * @param {string} curNamespace - The current namespace or class being checked.
     * @returns {string|undefined} The filename associated with the symbol, or undefined if the symbol is not found.
     */
    static getSymbolFilename(obj, symbol, curNamespace) {
        // Iterate <compound kind="class">
        if ('compound' in obj) {
            for (const compound of Array.isArray(obj['compound']) ? obj['compound'] : [obj['compound']]) {
                const kind = compound['@attributes']["kind"];
                if (kind !== 'class') {
                    continue
                }
                const name = compound['name'];
                if (symbol === name && 'filename' in compound) {
                    return compound['filename']
                }
            }
        }

        // Iterate <member kind="function">
        if ('member' in obj) {
            for (const member of Array.isArray(obj['member']) ? obj['member'] : [obj['member']]) {
                const kind = member['@attributes']["kind"];
                if (kind !== 'function') {
                    continue
                }
                const name = member['name']
                const qualifiedName = curNamespace ? curNamespace + "::" + name : name
                if (symbol === qualifiedName && 'anchorfile' in member) {
                    return member['anchorfile'] + (('anchor' in member && member['anchor']) ? `#${member["anchor"]}` : '')
                }
            }
        }

        // Recursively look for symbol in <compound kind="namespace"> or <compound kind="class">
        if ('compound' in obj) {
            for (const compound of Array.isArray(obj['compound']) ? obj['compound'] : [obj['compound']]) {
                const kind = compound['@attributes']["kind"];
                if (kind !== 'namespace' && kind !== 'class') {
                    continue
                }
                const name = compound['name'];
                if (symbol === name && 'filename' in compound) {
                    return compound['filename']
                }
                if (symbol.startsWith(name + '::')) {
                    const res = this.getSymbolFilename(compound, symbol, name)
                    if (res) {
                        return res
                    }
                }
            }
        }

        return undefined
    }

    /**
     * Splits a C++ symbol into its main symbol name, template parameters, and the rest of the symbol.
     *
     * This function is used to handle C++ symbols that are templates. It iterates over the symbol string,
     * keeping track of the level of nested templates. When it encounters a '<', it increments the level and
     * starts recording the template parameters. When it encounters a '>', it decrements the level and stops
     * recording the template parameters. The rest of the symbol is recorded after the template parameters.
     *
     * For instance, it splits 'std::vector<int>::iterator' into:
     * - main symbol name: 'std::vector'
     * - template parameters: ['int']
     * - rest: '::iterator'
     *
     * The extension uses this information to generate inline content with individual links
     * for the main symbol name, each template parameter, and the rest of the symbol.
     *
     * @param {string} symbol - The C++ symbol to split.
     * @returns {Object} An object with the main symbol name, the template parameters, and the rest of the symbol.
     */
    static splitCppSymbol(symbol) {
        let mainSymbolName = ''
        let curTemplate = ''
        let templateParameters = []
        let rest = ''
        let level = 0
        let inTemplateParameters = false

        for (let i = 0; i < symbol.length; i++) {
            let c = symbol[i];
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
                    rest = symbol.substring(i + 1)
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
}

module.exports = CppTagfilesExtension
