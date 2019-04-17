//
//  AppDelegate.swift
//  SPP_Integrated_App
//
//  Created by Akshay Chitale on 6/10/17.
//  Modifying by Tahsin Ahmed Chowdhury for Integation on 10/15/17
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import UIKit
import Foundation

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?


    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        // Override point for customization after application launch.

        // Set up audio settings
        audioController = AudioSettingsController()
        
        // Set up gains files
        htmlFile.onLaunch()
        
        // Initially, assume no imported file
        // application(_:open:options) gets called after applicationDidFinishLaunching
        importedFileErrorString = nil
        
        return true
    }
    
    func application(_ app: UIApplication, open url: URL, options: [UIApplicationOpenURLOptionsKey : Any] = [:]) -> Bool {
        // Ensure that directories exist
        htmlFile.onLaunch()
        
        // Import file, make error string not nil to show there was an import
        importedFileErrorString = ""
        importedFile = htmlFile.importGainsFile(url, errorString: &importedFileErrorString!)
        
        // If file was imported, set as current file and load settings
        if let gainsFile: String = importedFile {
            if let jsonObj = htmlFile.readGainsFile(gainsFile, errorString: &importedFileErrorString!) {
                if audioController.update(fromJSON: jsonObj, errorString: &importedFileErrorString!) {
                    audioController.gainsFile = gainsFile
                    // If success, clear error string 
                    importedFileErrorString = ""
                }
            }
        }
        
        return true
    }

    func applicationWillResignActive(_ application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
    }

    func applicationDidEnterBackground(_ application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    }

    func applicationWillEnterForeground(_ application: UIApplication) {
        // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
    }

    func applicationDidBecomeActive(_ application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    }

    func applicationWillTerminate(_ application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    }

}

