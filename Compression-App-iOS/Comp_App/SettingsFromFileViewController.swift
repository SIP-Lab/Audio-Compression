//
//  SettingsFromFileViewController.swift
//  Integrated_App
//
//  Created by Taghizadeh Alamdari, Nasim on 8/17/18.
//  Copyright © 2018 UT Dallas. All rights reserved.
//

import UIKit
import Foundation //to access NSObject or one of its subclasses

class SettingsFromFileViewController: UITableViewController {
    // local vars
    /// The gains files array to serve as the datasource for the tableView. This array should be equivalent to `htmlFile.gainsFiles`.
    var importFiles: [String]! = []

    override func viewDidLoad() {
        super.viewDidLoad()

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem
        
        // Notify when app enters foreground for updates
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(appActiveAction),
                                               name: NSNotification.Name.UIApplicationDidBecomeActive,
                                               object: UIApplication.shared)
        
        // Load files
        importFiles = htmlFile.gainsFiles
        tableView.reloadData()
        
        // Recognize long press to share json file
        let longPress: UILongPressGestureRecognizer =
            UILongPressGestureRecognizer(target: self,
                                         action: #selector(self.shareFile))
        view.addGestureRecognizer(longPress)
    }
    
    deinit {
        // Remove the notification
        NotificationCenter.default.removeObserver(self)
    }
    
    
    /// Shows an activity view controller to share the file.
    func shareFile(_ longPress: UILongPressGestureRecognizer) {
        if longPress.state == UIGestureRecognizerState.began {
            let point: CGPoint = longPress.location(in: self.view)
            if let indexPath: IndexPath = tableView.indexPathForRow(at: point) {
                // Cell at indexPath is file to open
                if let name: String = tableView.cellForRow(at: indexPath)?.textLabel?.text {
                    let share: [Any] = [htmlFile.getURL(of: name) as Any]
                    let activityController = UIActivityViewController(activityItems: share, applicationActivities: nil)
                    self.present(activityController, animated: true, completion: nil)
                }
            }
        }
    }
    
    /// Shows an error alert if an error occurs when loading files.
    ///
    /// - Parameter message: The error message to show in the alert.
    func showErrorAlert(_ message: String) {
        let alert: UIAlertController = UIAlertController(title: "Load Settings File Error",
                                                         message: message,
                                                         preferredStyle: .alert)
        let ok = UIAlertAction(title: "OK", style: .default, handler: {
            UIAlertAction in
        })
        alert.addAction(ok)
        self.present(alert, animated: true, completion: nil)
    }
    
    /// Shows an error alert if an error occurs when importing files.
    ///
    /// - Parameter message: The error message to show in the alert.
    func showImportErrorAlert(_ message: String) {
        let alert: UIAlertController = UIAlertController(title: "Import File Error",
                                                         message: message,
                                                         preferredStyle: .alert)
        let ok = UIAlertAction(title: "OK", style: .default, handler: {
            UIAlertAction in
        })
        alert.addAction(ok)
        self.present(alert, animated: true, completion: nil)
    }
    
    
    /// Updates file list when app becomes active.
    ///
    /// Selected to be called so that the imported file is shown.
    func appActiveAction(_ note: NSNotification) {
        if(self.isViewLoaded && self.view.window != nil) {
            // Show error from import if there is one pending
            if let importError = importedFileErrorString {
                if(importError != "") {
                    showImportErrorAlert(importError)
                    // Clear error string
                    importedFileErrorString = nil
                }
            }
            importFiles = htmlFile.gainsFiles
            tableView.reloadData()
        }
    }
    
    /// Shows a confirmation alert to delete a file.
    ///
    /// - Parameters:
    ///   - name: The name of the file to delete, without the `.json` extension.
    ///   - indexPath: The path to the row of the deleted file in `self.tableView`.
    func showDeleteConfirm(_ name: String, indexPath: IndexPath) {
        // First, alert user with file name
        let alert: UIAlertController = UIAlertController(title: "Delete \"\(name)\"?",
            message: "This action cannot be undone.",
            preferredStyle: .alert)
        let ok = UIAlertAction(title: "Delete", style: .destructive, handler: {
            UIAlertAction in
            var errorString: String = ""
            if let delName: String = htmlFile.deleteGainsFile(name, errorString: &errorString),
                let pos: Int = self.importFiles.index(of: delName) {
                // If deleted current gains file, set file name to Custom
                if delName == audioController.gainsFile {
                    audioController.gainsFile = "Custom"
                }
                self.importFiles.remove(at: pos)
                self.tableView.deleteRows(at: [indexPath], with: .automatic)
            }
            else {
                self.showErrorAlert(errorString)
                self.tableView.deselectRow(at: indexPath, animated: true)
            }
        })
        let cancel = UIAlertAction(title: "Cancel", style: .cancel, handler: {
            UIAlertAction in
            // Does nothing
        })
        alert.addAction(ok)
        alert.addAction(cancel)
        self.present(alert, animated: true, completion: nil)
    }

    // MARK: - Table view data source
    override func numberOfSections(in tableView: UITableView) -> Int {
         // #warning Incomplete implementation, return the number of sections
        return 1
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return importFiles.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell: UITableViewCell = tableView.dequeueReusableCell(withIdentifier: "fileCell", for: indexPath)
        // Fill in cell info, checkmark if applicable
        cell.textLabel?.text = importFiles[indexPath.row]
        if importFiles[indexPath.item] == audioController.gainsFile {
            cell.accessoryType = UITableViewCellAccessoryType.checkmark
        }
        else {
            cell.accessoryType = UITableViewCellAccessoryType.none
        }
        return cell
    }
    
    // To delete rows from table view
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == UITableViewCellEditingStyle.delete {
            // Delete row code
            if let name = tableView.cellForRow(at: indexPath)?.textLabel?.text {
                showDeleteConfirm(name, indexPath: indexPath)
            }
        }
    }
    

    
    // Selecting row which functions as a button
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        // Read settings file from that file, if successful then update gainsFile
        if let name: String =  tableView.cellForRow(at: indexPath)?.textLabel?.text {
            var errorString: String = ""
            var success: Bool = false;
            // errorString if action fails followed by action
            // success only set to true if all if statements succeed
            if let jsonObj = htmlFile.readGainsFile(name, errorString: &errorString) {
                success = audioController.update(fromJSON: jsonObj, errorString: &errorString) //if successful then update gainsFile
            }
            
            if success {
                // Set name and go back to prev menu
                audioController.gainsFile = name
                _ = self.navigationController?.popViewController(animated: true)
       
            }
            else {
                showErrorAlert(errorString)
                tableView.deselectRow(at: indexPath, animated: true)
            }
        }
    }
    //################### End of functions ############################
    /* override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }*/


    /*
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "reuseIdentifier", for: indexPath)

        // Configure the cell...

        return cell
    }
    */

    /*
    // Override to support conditional editing of the table view.
    override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == .delete {
            // Delete the row from the data source
            tableView.deleteRows(at: [indexPath], with: .fade)
        } else if editingStyle == .insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }    
    }
    */

    /*
    // Override to support rearranging the table view.
    override func tableView(_ tableView: UITableView, moveRowAt fromIndexPath: IndexPath, to: IndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(_ tableView: UITableView, canMoveRowAt indexPath: IndexPath) -> Bool {
        // Return false if you do not want the item to be re-orderable.
        return true
    }
    */

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

} // End of class SettingsFromFileViewController: UITableViewController
