//
//  htlmFile.swift
//  Integrated_App
//
//  Created by Akshay Chitale on 7/10/17.
//  Created by Taghizadeh Alamdari, Nasim on 8/14/18.
//  Copyright © 2018 UT Dallas. All rights reserved.
//

import UIKit
import Foundation

/// Globally availible imported file name, set in `AppDelegate.swift` when a file is imported from another app.
var importedFile: String? = nil

/// Globally availible error when importing file, set in `AppDelegate.swift` when a file is imported from another app.
var importedFileErrorString: String? = nil

/// A class of static functions to manage gains file handling, such as reading, saving, and deleting files.
class htmlFile {
    /// The URL of this application's `Documents` directory.
    static var documentsDirectory: URL {
        get {
            let paths = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)
            let documentsDirectory = paths[0]
            return documentsDirectory
        }
    }
    
    /// The URL of the user gains directory, at `Documents/FromHTML`.
    static var gainsDirectory: URL {
        get {
            return documentsDirectory.appendingPathComponent("FromHTML", isDirectory: true)
        }
    }
    
    
    /// The URL of the JSON text file which stores settings across app launches, at `Documents/GainsSettings.json`.
    static var gainsSettingsFile: URL {
        get {
            return documentsDirectory.appendingPathComponent("GainsSettings.json", isDirectory: false)
        }
    }
    
    /// The array of bundle gains to ignore when presenting gains to the user, stored in `GainsFiles.gainsSettingsFile`.
    static var ignoreBundleGains: [String] {
        get {
            var ignoreFiles: [String] = []
            do {
                let content: Data = try Data(contentsOf: htmlFile.gainsSettingsFile)
                let jsonContent = try JSONSerialization.jsonObject(with: content, options: [])
                if let jsonObj = jsonContent as? [String: Any],
                    let ignores = jsonObj["ignoreBundleGains"] as? [Any],
                    let strignores = ignores as? [String] {
                    ignoreFiles = strignores
                }
            }
            catch let error {
                // Should not happen because onLaunch() ensured GainsSettings.json exists
                // This class ensures JSON format
                print("Read ignore bundle gains error")
                print(error)
            }
            return ignoreFiles
        }
    }
    
    /// An array of the bundle gains availible to the user, ignoring the files in `GainsFiles.ignoreBundleGains`.
    static var bundleGains: [String] {
        get {
            let ignoreGains: [String] = htmlFile.ignoreBundleGains
            var gainsList: [String] = []
            if let gainsURLs: [URL] = Bundle.main.urls(forResourcesWithExtension: "json", subdirectory: "FromHTML") {
                // For each, show only if gains not ignored
                for file: URL in gainsURLs {
                    let fname: String = file.lastPathComponent.replacingOccurrences(of: ".json", with: "", options: String.CompareOptions.caseInsensitive, range: nil)
                    if !ignoreGains.contains(fname) {
                        gainsList.append(fname)
                    }
                }
            }
            else {
                // Should not happen because Gains resource directory exists
                print("Error loading bundle gains files")
            }
            return gainsList
        }
    }
    
    /// An array of the user gains files in `GainsFiles.gainsDirectory`.
    static var userGains: [String] {
        get {
            var gainsList: [String] = []
            do {
                let userGainsURLs: [URL] = try FileManager.default.contentsOfDirectory(at: htmlFile.gainsDirectory, includingPropertiesForKeys: nil, options: [])
                // For each, show only if file is a JSON file
                for file: URL in userGainsURLs {
                    if file.pathExtension == "json" {
                        let fname: String = file.lastPathComponent.replacingOccurrences(of: ".json", with: "", options: String.CompareOptions.caseInsensitive, range: nil)
                        gainsList.append(fname)
                    }
                }
            }
            catch let error {
                // Should not happen because onLaunch() ensured that Documents/Gains exists
                print("Error loading user gains files")
                print(error)
            }
            return gainsList
        }
    }
    
    /// An array of all gains files availible, a concatenation of `GainsFiles.bundleGains` and `GainsFiles.userGains`.
    static var gainsFiles: [String] {
        get {
            // User gains + bundle gains
            var gainsList: [String] = htmlFile.userGains
            gainsList.append(contentsOf: htmlFile.bundleGains)
            return gainsList
        }
    }
    
    
    /// Ensures that the directory `GainsFiles.gainsDirectory` and the file `GainsFiles.gainsSettingsFile` both exist, and that the latter is in proper JSON format.
    ///
    /// This function should be called before any other member or function in this class is called, i.e. in application delegate when the app did finish launching
    static func onLaunch() {
        // Make gains folder if DNE
        var TRUE: ObjCBool = ObjCBool(true)
        if !FileManager.default.fileExists(atPath: htmlFile.gainsDirectory.path, isDirectory: &TRUE) {
            do {
                try FileManager.default.createDirectory(at: htmlFile.gainsDirectory, withIntermediateDirectories: false, attributes: nil)
            }
            catch let error {
                print(error)
            }
        }
        // Make settings file if DNE
        var FALSE: ObjCBool = ObjCBool(false)
        if !FileManager.default.fileExists(atPath: htmlFile.gainsSettingsFile.path, isDirectory: &FALSE) {
            let defaultSettingsFile: String = "{\n" +
                "\t\"ignoreBundleGains\": []\n" +
            "}\n"
            FileManager.default.createFile(atPath: htmlFile.gainsSettingsFile.path, contents: defaultSettingsFile.data(using: .utf8), attributes: nil)
        }
    }
    
    /// Gets the URL of a gains file specified.
    ///
    /// - Parameter name: The name of the gains file of which to return the URL.
    /// - Returns: The URL of the gains file specified, or `nil` if the file was not found.
    static func getURL(of: String) -> URL? {
        if htmlFile.userGains.contains(of) {
            return htmlFile.gainsDirectory.appendingPathComponent("\(of).json")
        }
        else if htmlFile.bundleGains.contains(of) {
            return Bundle.main.url(forResource: of, withExtension: ".json", subdirectory: "FromHTML")
        }
        return nil
    }
    
    /// Reads JSON from a gains file
    ///
    /// This function only returns the main JSON object in the file.
    ///
    /// - Parameters:
    ///   - name: The name of the file to read, without the `.json` extension.
    ///   - errorString: An inout string in which to store error information should one occur.
    /// - Returns: The JSON object read if successful, or `nil` if an error occured.
    static func readGainsFile(_ name: String, errorString: inout String) -> [String: Any]? {
        errorString = "File \"\(name)\" does not exist"
        // Try reading from user gains, if fail then try from bundle gains, if fail then error occurred
        if htmlFile.userGains.contains(name) {
            do {
                let path: URL = htmlFile.gainsDirectory.appendingPathComponent("\(name).json")
                
                errorString = "Could not read contents of \(name)"
                let content: Data = try Data(contentsOf: path)
                errorString = "Invalid JSON in file \(name)"
                let jsonContent = try JSONSerialization.jsonObject(with: content, options: [])
                
                errorString = "Invalid main JSON object in file \(name)"
                if let jsonObj = jsonContent as? [String: Any] {
                    // Valid JSON in file
                    return jsonObj
                }
            }
            catch let error {
                print(errorString)
                print(error)
            }
        }
        else if htmlFile.bundleGains.contains(name) {
            errorString = "Could not open \(name)"
            if let file: URL = Bundle.main.url(forResource: name, withExtension: ".json", subdirectory: "FromHTML") {
                do {
                    errorString = "Could not read contents of \(name)"
                    let content: Data = try Data(contentsOf: file)
                    
                    errorString = "Invalid JSON in file \(name)"
                    let jsonContent = try JSONSerialization.jsonObject(with: content, options: [])
                    
                    errorString = "Invalid main JSON object in file \(name)"
                    if let jsonObj = jsonContent as? [String: Any] {
                        return jsonObj
                    }
                }
                catch let error {
                    print(errorString)
                    print(error)
                }
            }
        }
        // Return nil if error
        return nil
    }
    
    
    /// Saves a gains file
    ///
    /// The file is saved as a `.json` text file in `GainsFiles.gainsDirectory`
    ///
    /// - Parameters:
    ///   - name: The name of the file to save.
    ///   - numGains: The number of gains in each gains array.
    ///   - LMHGains: The "Low", "Medium", and "High" gains, at indicies 0, 1, and 2, respectively.
    ///   - errorString: An inout string in which to store error information should one occur.
    /// - Returns: The name of the file deleted without the `.json` extension if successful, or `nil` if an error occured.
    static func saveGainsFile(_ name: String, numGains: Int, LMHGains: [[Float]], errorString: inout String) -> String? {
        errorString = "Unknown error"
        // Need to have entered a file name, remove .json from name if user added it
        let saveName: String = name.replacingOccurrences(of: ".json", with: "", options: String.CompareOptions.caseInsensitive, range: nil)
        // Make sure file doesn't already exist
        errorString = "File \(saveName).json already exists"
        if !htmlFile.gainsFiles.contains(saveName) {
            // Make sure name is not "Default" or "Custom"
            errorString = "Cannot use reserved name \(saveName)"
            if saveName != "Default" && saveName != "Custom" {
                // Check params
                errorString = "Low, Medium, High gains not supplied to save"
                if LMHGains.count == 3 {
                    var gainLen: Bool = true
                    for i: [Float] in LMHGains {
                        if i.count != numGains {
                            gainLen = false
                        }
                    }
                    errorString = "Gains length not equal to \(numGains)"
                    if gainLen {
                        // Gains data
                        var gainsArrs: [String] = []
                        for gD:[Float] in LMHGains {
                            var tempStr = "["
                            var comma = false
                            for i in gD {
                                if comma {
                                    tempStr.append(", ")
                                }
                                tempStr.append("\(i)")
                                comma = true
                            }
                            tempStr.append("]")
                            gainsArrs.append(tempStr)
                        }
                        // String of JSON data, save file
                        let JSONData: String = "{\n" +
                            "\t\"numGains\": \(numGains),\n" +
                            "\t\"gains\": {\n" +
                            "\t\t\"Low\":\t\t\(gainsArrs[0]),\n" +
                            "\t\t\"Medium\":\t\(gainsArrs[1]),\n" +
                            "\t\t\"High\":\t\t\(gainsArrs[2])\n" +
                        "\t}\n}\n"
                        let newFileURL: URL = htmlFile.gainsDirectory.appendingPathComponent("\(saveName).json")
                        do {
                            errorString = "Error writing JSON to file"
                            try JSONData.write(to: newFileURL, atomically: true, encoding: String.Encoding.utf8)
                            // Once written, return file name for file name in audioController, no error string
                            return saveName
                        }
                        catch let error {
                            print(errorString)
                            print(error)
                        }
                    }
                }
            }
        }
        // Return nil if error
        return nil
    }
    
    
    /// Deletes a gains file
    ///
    /// For user gains files, the file is actually deleted. For bundle gains, the file name is added to `GainsFiles.ignoreBundleGains`.
    ///
    /// - Parameters:
    ///   - name: The name of the file to delete, without the `.json` extension.
    ///   - errorString: An inout string in which to store error information should one occur.
    /// - Returns: The name of the file deleted without the `.json` extension if successful, or `nil` if an error occured.
    static func deleteGainsFile(_ name: String, errorString: inout String) -> String? {
        errorString = "File \"\(name)\" does not exist"
        // If name in user gains, delete file, else if in bundle gains, put in ignore list, else file does not exist
        if htmlFile.userGains.contains(name) {
            do {
                errorString = "Error deleting user gains file \(name)"
                try FileManager.default.removeItem(at: htmlFile.gainsDirectory.appendingPathComponent("\(name).json"))
                // Return deleted file name
                return name
            }
            catch let error {
                print(errorString)
                print(error)
            }
        }
        else if htmlFile.bundleGains.contains(name) {
            var ignore: [String] = htmlFile.ignoreBundleGains
            ignore.append(name)
            var JSONData: String = "{\n\t\"ignoreBundleGains\": [\n"
            for i: String in ignore {
                JSONData.append("\t\t\"\(i)\",\n")
            }
            JSONData.append("\t]\n}\n")
            do {
                errorString = "Error writing JSON to gains settings file"
                try JSONData.write(to: htmlFile.gainsSettingsFile, atomically: true, encoding: String.Encoding.utf8)
                // Return ignored file name
                return name
            }
            catch let error {
                print(errorString)
                print(error)
            }
        }
        // Return nil if error
        return nil
    }
    
    
    /// Imports a gains file into `GainsFiles.gainsDirectory`.
    ///
    /// This function is for use when importing a file from another application to this one.
    ///
    /// - Parameters:
    ///   - file: The URL of the file to import.
    ///   - errorString: An inout string in which to store error information should one occur.
    /// - Returns: The name of the file imported without the `.json` extension if successful, or `nil` if an error occured.
    static func importGainsFile(_ file: URL, errorString: inout String) -> String? {
        errorString = "Unknown error"
        // Move file to GainsFiles.gainsDirectory, then clear Inbox directory
        do {
            errorString = "Error reaching imported file"
            let name: String = file.lastPathComponent.replacingOccurrences(of: ".json", with: "", options: String.CompareOptions.caseInsensitive, range: nil)
            let newURL: URL = htmlFile.gainsDirectory.appendingPathComponent("\(name).json")
            
            // Make sure name is not "Default" or "Custom"
            errorString = "Cannot use reserved name \(name)"
            if name != "Default" && name != "Custom" {
                
                errorString = "Error moving imported file to Gains directory"
                try FileManager.default.copyItem(at: file, to: newURL)
                
                errorString = "Error clearing Inbox directory"
                let inboxDir: URL = htmlFile.documentsDirectory.appendingPathComponent("Inbox", isDirectory: true)
                let inboxURLs: [URL] = try FileManager.default.contentsOfDirectory(at: inboxDir, includingPropertiesForKeys: nil, options: [])
                // For each, show only if file is a JSON file
                for inboxFile: URL in inboxURLs {
                    errorString = "Error clearing \(inboxFile.lastPathComponent) from Inbox directory"
                    try FileManager.default.removeItem(at: inboxFile)
                }
                
                return name
            }
        }
        catch let error {
            print(error)
        }
        // Return nil if error
        return nil
    }
}
