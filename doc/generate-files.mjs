#!/usr/bin/env node

//
// Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

'use strict';
import {execSync} from 'child_process'
import fs from 'fs'
import path from 'path'
import os from 'os'
import request from 'sync-request'

/*
    Helper functions
 */
function mkdirp(dir) {
    if (!fs.existsSync(dir)) {
        mkdirp(path.dirname(dir))
        fs.mkdirSync(dir)
    }
}

function findExecutable(executableName) {
    if (Array.isArray(executableName)) {
        for (const name of executableName) {
            const result = findExecutable(name)
            if (result) {
                return result
            }
        }
        return undefined
    }
    try {
        const whichCommand = (process.platform === 'win32') ? 'where' : 'which';
        const cmd = `${whichCommand} ${executableName}`
        const res = execSync(cmd, {encoding: 'utf-8'}).trim();
        return res
    } catch (error) {
        return undefined
    }
}

function mkTmpDir() {
    let tempDir = undefined
    try {
        tempDir = fs.mkdtempSync(os.tmpdir())
    } catch (error) {
        console.error(`Could not create temporary directory with fs.mkdtempSync(os.tmpdir()): ${error}.`)
        try {
            console.error(`Trying to create temporary directory in fs.mkdtempSync('~/tmp')...`)
            tempDir = fs.mkdtempSync(path.join(os.homedir(), 'tmp'));
        } catch (error) {
            console.error(`Could not create temporary directory with fs.mkdtempSync('~/tmp'): ${error}.`)
            try {
                console.error(`Trying to create temporary directory in fs.mkdtempSync(path.join(cwd, 'tmp'))...`)
                tempDir = fs.mkdtempSync(path.join(cwd, 'tmp'));
                console.log(`Created temporary directory ${tempDir}`)
            } catch (error) {
                console.error(`Could not create temporary directory: ${error}`)
            }
        }
    }
    return tempDir
}

let tempDir = undefined

function downloadAndDecompress(downloadUrl, downloadPath, extractPath) {
    try {
        console.log(`Downloading ${downloadUrl} to ${downloadPath}...`);
        const response = request('GET', downloadUrl)
        fs.writeFileSync(downloadPath, response.getBody());
        console.log(`File ${downloadUrl} downloaded successfully to ${downloadPath}.`);
        if (path.extname(downloadPath) === '.7z') {
            const cmd = `7z x ${downloadPath} -o${extractPath}`
            console.log(`Decompressing with command: ${cmd}`)
            execSync(cmd, {stdio: 'inherit'})
        } else if (/\.tar\.gz$/.test(downloadPath)) {
            mkdirp(extractPath)
            const cmd = `tar -vxzf ${downloadPath} -C ${extractPath}`
            console.log(`Decompressing with command: ${cmd}`)
            execSync(cmd, {stdio: 'inherit'})
        }
        console.log(`File decompressed successfully to ${extractPath}.`);
    } catch (error) {
        console.error('Error:', error.message);
    }
}

/*
    Find C++ compilers
    Clang++ is preferred over g++ and cl for MrDocs
 */
let cxxCompiler = findExecutable(['clang++', 'g++', 'cl']) || process.env.CXX_COMPILER || process.env.CXX
if (cxxCompiler && process.platform === "win32") {
    // Replace "\" with "/" in CXX_COMPILER
    cxxCompiler = cxxCompiler.replace(/\\/g, '/')
}
const cxxCompilerName = path.basename(cxxCompiler).replace(/\.exe$/, '')
let cCompiler = findExecutable(['clang', 'gcc', 'cl']) || process.env.C_COMPILER || process.env.CC
if (cCompiler && process.platform === "win32") {
    // Replace "\" with "/" in CXX_COMPILER
    cCompiler = cCompiler.replace(/\\/g, '/')
}
const cCompilerName = path.basename(cCompiler).replace(/\.exe$/, '')

console.log(`C++ compiler: ${cxxCompilerName} (${cxxCompiler})`)
console.log(`C compiler: ${cCompilerName} (${cCompiler})`)

/*
    Extract CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES
 */
const cwd = process.cwd()
const buildDirectory = path.join(cwd, 'doc', 'build')
const implicitTestDir = path.join(buildDirectory, `__implicit_dirs_${process.platform}_${cCompilerName || 'default'}_test__`)
const implicitTestCMakeList = path.join(implicitTestDir, 'CMakeLists.txt')
const outputFilePath = path.join(implicitTestDir, 'include_dirs.txt')
const implicitTestBuildDir = path.join(implicitTestDir, '__build__')

mkdirp(implicitTestDir)
fs.writeFileSync(implicitTestCMakeList, 'cmake_minimum_required(VERSION 3.22)\nproject(get_implicit_dirs)\nfile(WRITE "include_dirs.txt" "\${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}")\n')
mkdirp(implicitTestBuildDir)
let implicitCmakeConfigureCmd = 'cmake'
if (process.platform === "win32") {
    // The default generator for cl.exe is Visual Studio, but it doesn't create compile_commands.json
    // To use the ninja generator, you'll need to add
    // C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64
    // to your PATH
    implicitCmakeConfigureCmd += ' -G "Ninja"'
}
if (cxxCompiler) {
    implicitCmakeConfigureCmd += ` -D CMAKE_CXX_COMPILER="${cxxCompiler}"`
}
if (cCompiler) {
    implicitCmakeConfigureCmd += ` -D CMAKE_C_COMPILER="${cCompiler}"`
}
implicitCmakeConfigureCmd += ' ..'
console.log(`Extracting implicit include dirs with: ${implicitCmakeConfigureCmd}`)
execSync(implicitCmakeConfigureCmd, {cwd: implicitTestBuildDir})
const defaultIncludes = fs.readFileSync(outputFilePath, 'utf-8').trim()
console.log(`Default includes: ${defaultIncludes}`)

/*
    Download Boost
 */

// Check if we need to download boost. This is usually the case in CI, but we want to reuse the local boost directory
// in local builds.
const librariesJsonPath = path.join(cwd, 'meta', 'libraries.json')
const librariesJson = JSON.parse(fs.readFileSync(librariesJsonPath, 'utf-8'))
const module = librariesJson['key']
const self = module || path.basename(cwd)
console.log(`self: ${self}`)
// Go two levels up from cwd
const cwdParent = path.dirname(cwd)
const cwdParentParent = path.dirname(cwdParent)

// Check if cwdParentParent contains the files "CMakeLists.txt" "Jamroot" "boost-build.jam" "bootstrap.sh" "libs"
const boostFiles = ['CMakeLists.txt', 'Jamroot', 'boost-build.jam', 'bootstrap.sh', 'libs']
let boostDirExists = true
for (const file of boostFiles) {
    const filePath = path.join(cwdParentParent, file)
    console.log(`Checking if ${filePath} exists`)
    if (!fs.existsSync(filePath)) {
        boostDirExists = false
        break
    }
}
let boostDir = undefined
if (boostDirExists) {
    boostDir = cwdParentParent
    console.log(`Found Boost at ${cwdParentParent}`)
} else {
    // Identify branch of this git repository
    let boostBranch = 'develop'
    const branch = execSync('git rev-parse --abbrev-ref HEAD', {encoding: 'utf-8'}).trim()
    // Check if branch matches the boost-1.\d+.\d+ pattern
    const boostBranchRegex = /^boost-1\.\d+\.\d+$/
    if (branch === 'master' || branch === 'develop' || boostBranchRegex.test(branch)) {
        boostBranch = branch
    }

    // Clone boost in a temporary directory
    if (!tempDir) {
        tempDir = mkTmpDir()
    }
    console.log(`Created temporary directory ${tempDir}`)
    let boostDownloadCmd = `git clone https://github.com/boostorg/boost.git --depth 1 --branch ${boostBranch}`
    execSync(boostDownloadCmd, {cwd: tempDir})
    boostDir = path.join(tempDir, 'boost')
    console.log(`Initializing submodules in ${boostDir}`)
    execSync('git submodule update --init --recursive', {cwd: boostDir})
    console.log(`Cloned Boost to ${boostDir}`)
}

/*
    Configure Boost.URL with CMAKE_EXPORT_COMPILE_COMMANDS=ON
 */
const compileCommandsDir = path.join(buildDirectory, `__compile_commands_${process.platform}_${cCompilerName || 'default'}__`)
const compileCommandsPath = path.join(compileCommandsDir, 'compile_commands.json')
const SELF = self.toUpperCase()
const SELFDir = cwd
let cmakeConfigureCmd = `cmake`
if (process.platform === "win32") {
    // The default generator for cl.exe is Visual Studio, but it doesn't create compile_commands.json
    cmakeConfigureCmd += ' -G "Ninja"'
}
cmakeConfigureCmd += ` -D BOOST_${SELF}_BUILD_TESTS=OFF -D BOOST_${SELF}_BUILD_EXAMPLES=OFF -D BUILD_TESTING=OFF`
if (defaultIncludes) {
    // On Linux compilers, we need the implicit directories made explicit so that they are
    // reflected in the compile commands or mrdocs later find the headers it needs
    cmakeConfigureCmd += ` -D CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES="${defaultIncludes}"`
}
if (cxxCompiler) {
    cmakeConfigureCmd += ` -D CMAKE_CXX_COMPILER="${cxxCompiler}"`
}
if (cCompiler) {
    cmakeConfigureCmd += ` -D CMAKE_C_COMPILER="${cCompiler}"`
}
cmakeConfigureCmd += ` -D CMAKE_EXPORT_COMPILE_COMMANDS=ON`
cmakeConfigureCmd += ` -D BOOST_SRC_DIR="${boostDir}"`
mkdirp(compileCommandsDir)
const SELFDirRelative = path.relative(compileCommandsDir, SELFDir)
cmakeConfigureCmd += ` "${SELFDirRelative}"`
console.log(`Configuring Boost.${SELF} with command: ${cmakeConfigureCmd}`)
execSync(cmakeConfigureCmd, {cwd: compileCommandsDir})
if (!fs.existsSync(compileCommandsPath)) {
    console.error(`Could not find ${compileCommandsPath}`)
    process.exit(1)
}
console.log(`Configure commands written to ${compileCommandsPath}`)

/*
    Find and run MrDocs to generate the documentation

    Install MrDocs with:
        - Linux:
            wget https://github.com/cppalliance/mrdocs/releases/download/develop-release/MrDocs-1.0.0-Linux.tar.gz
            sudo tar -xzf MrDocs-1.0.0-Linux.tar.gz -C /usr/local --strip-components=1
        - Windows (Powershell):
            Invoke-WebRequest -Uri 'https://github.com/cppalliance/mrdocs/releases/download/develop-release/MrDocs-1.0.0-win64.7z' -OutFile 'MrDocs-1.0.0-win64.7z'
            7z x -o"C:\Users\$env:USERNAME\Applications" 'MrDocs-1.0.0-win64.7z'
            ( Adapt the destination path as needed )

 */
let mrDocsExec = findExecutable(['mrdocs', 'mrdox'])
if (!mrDocsExec) {
    console.log(`Could not find MrDocs. Downloading...`)
    if (!tempDir) {
        tempDir = mkTmpDir()
    }
    const downloadFilename = process.platform === "win32" ? 'MrDocs-1.0.0-win64.7z' : 'MrDocs-1.0.0-Linux.tar.gz'
    const downloadExtractDir = process.platform === "win32" ? 'MrDocs-1.0.0-win64' : 'MrDocs-1.0.0-Linux'
    const downloadExtractBinFilename = process.platform === "win32" ? 'mrdocs.exe' : 'mrdocs'
    downloadAndDecompress(
        `https://github.com/cppalliance/mrdocs/releases/download/develop-release/${downloadFilename}`,
        path.join(tempDir, downloadFilename),
        path.join(tempDir, 'MrDocs')
    )
    mrDocsExec = path.join(tempDir, 'MrDocs', downloadExtractDir, 'bin', downloadExtractBinFilename)
    if (!fs.existsSync(mrDocsExec)) {
        console.error(`Could not find MrDocs at ${mrDocsExec}`)
        process.exit(1)
    }
}
console.log(`Found MrDocs at ${mrDocsExec}`)
const mrDocsBinDir = path.dirname(mrDocsExec)
const mrDocsShareDir = path.join(mrDocsBinDir, '..', 'share')
let mrDocsAddonsDir = [
    path.join(mrDocsShareDir, 'mrdocs', 'addons'),
    path.join(mrDocsShareDir, 'mrdocs', 'mrdocs', 'addons'),
    path.join(mrDocsShareDir, 'mrdox', 'addons'),
    path.join(mrDocsShareDir, 'mrdox', 'mrdox', 'addons')].find(fs.existsSync)
// Reference goes to another module so that its relative links work
// Antora does not support relative xrefs: https://gitlab.com/antora/antora/-/issues/428
const mrDocsOutputDir = path.join(buildDirectory, 'generated-files', 'modules', 'reference', 'pages')
const mrDocsConfigPath = path.join(cwd, 'doc', 'mrdocs.yml')
const mrDocsCmd = `${mrDocsExec} --config="${mrDocsConfigPath}" "${compileCommandsPath}" --addons="${mrDocsAddonsDir}" --output="${mrDocsOutputDir}"`
console.log(`Generating documentation with command: ${mrDocsCmd}`)
execSync(mrDocsCmd)

