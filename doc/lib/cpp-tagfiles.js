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

        // https://www.npmjs.com/package/@antora/logger
        // https://github.com/pinojs/pino/blob/main/docs/api.md
        this.logger = this.context.getLogger('cpp-tagfile-extension')
        this.logger.info('Registering cpp-tagfile-extension')

        this.config = config
        this.playbook = playbook
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
     * The first tagfile is always <script directory>/cpp_tags/cppreference-doxygen-web.tag.xml.
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

        // The first tagfile is always <script directory>/cpp_tags/cppreference-doxygen-web.tag.xml
        const defaultTagfilesDir = path.join(__dirname, 'cpp_tagfiles');
        this.tagfiles.push({
            file: path.join(defaultTagfilesDir, 'cppreference-doxygen-web.tag.xml'),
            baseUrl: 'https://en.cppreference.com/w/'
        })

        // Iterate files in playbook.
        const playbookFiles = this.config.files || []
        playbookFiles.forEach(tagfile => {
            this.tagfiles.push({file: path.join(playbook.dir, tagfile.file), baseUrl: tagfile.baseUrl})
        })

        // Look for tagfiles set in the components
        for (const componentVersionBucket of contentAggregate.slice()) {
            const {origins = []} = componentVersionBucket
            for (const origin of origins) {
                const {descriptor, worktree} = origin
                let tagFilesConfig = descriptor?.ext?.cppTagfiles?.files || []
                for (const tagfile of tagFilesConfig) {
                    this.tagfiles.push({file: path.join(worktree, tagfile.file), baseUrl: tagfile.baseUrl})
                }
            }
        }

        // Remove duplicates
        this.tagfiles = this.tagfiles.filter((tagfile, index, self) => self.findIndex(t => t.file === tagfile.file) === index)
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
            const {file, baseUrl} = tagfile
            if (!fs.existsSync(file)) {
                this.logger.error(`Tagfile not found: ${file}`)
                return
            }
            const xml = fs.readFileSync(file, 'utf8');
            tagfile.doc = parser.parse(xml);
        })
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
                        const link = extensionSelf.getSymbolLink(he.decode(target))
                        if (link) {
                            return self.createInline(parent, 'quoted', link, {type: 'monospaced'})
                        }
                        return self.createInline(parent, 'quoted', target, {type: 'monospaced'})
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
     * @returns {undefined|string} The URL for the symbol, or undefined if the symbol is not found.
     */
    getSymbolLink(symbol) {
        if (CppTagfilesExtension.isFundamentalType(symbol)) {
            return `https://en.cppreference.com/w/cpp/language/types[${symbol},window=_blank]`
        }

        // Handle links to include files
        const isIncludeFile = symbol.startsWith('"') && symbol.endsWith('"') ||
            symbol.startsWith('<') && symbol.endsWith('>');
        if (isIncludeFile) {
            for (const tagfile of this.tagfiles) {
                const filename = CppTagfilesExtension.getFileFilename(tagfile.doc['tagfile'], symbol)
                if (filename) {
                    return `https://en.cppreference.com/w/${filename}[${he.encode(symbol)},window="_blank"]`
                }
            }
            return undefined
        }

        // Handle symbols
        if (!symbol.includes('<')) {
            // Handle symbols that are not templates
            for (const tagfile of this.tagfiles) {
                const filename = CppTagfilesExtension.getSymbolFilename(tagfile.doc['tagfile'], symbol, '')
                if (filename !== undefined) {
                    return `${tagfile.baseUrl}${filename}[${he.encode(symbol)},window="_blank"]`
                }
            }
        } else {
            // Handle symbols that are templates
            const {mainSymbolName, templateParameters, rest} = CppTagfilesExtension.splitCppSymbol(symbol)
            let fullLink = ''
            const mainLink = this.getSymbolLink(mainSymbolName)
            if (mainLink) {
                fullLink = mainLink
            } else {
                fullLink = he.encode(mainSymbolName)
            }
            fullLink += he.encode('<')
            let isFirst = true
            for (const templateParameter of templateParameters) {
                const templateParameterLink = this.getSymbolLink(templateParameter)
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
                const restLink = this.getSymbolLink(mainSymbolName + rest)
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
    static isFundamentalType(type) {
        return ['bool', 'char', 'char8_t', 'char16_t', 'char32_t', 'wchar_t', 'int', 'signed', 'unsigned', 'short', 'long', 'float', 'true', 'false', 'double', 'void'].includes(type)
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
        const tagFileObj = doc['tagfile']
        for (const compound of tagFileObj['compound']) {
            if (compound['name'] === targetFilename) {
                return compound['filename']

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
            for (const compound of obj['compound']) {
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
            for (const member of obj['member']) {
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

        // <compound kind="namespace">
        if ('compound' in obj) {
            for (const compound of obj['compound']) {
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
