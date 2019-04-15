//
//  NoiseClassifierSettingsTableViewController.swift
//  Integrated_App
//
//  Created by Taghizadeh Alamdari, Nasim on 7/23/18.
//  Copyright © 2018 UT Dallas. All rights reserved.
//

import UIKit

class NoiseClassifierSettingsTableViewController: UITableViewController {

    
    //let BANDGAINS : [Float] =  [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0];
    
    @IBOutlet weak var LabelUserBand1: UILabel!
    @IBOutlet weak var LabelUserBand2: UILabel!
    @IBOutlet weak var LabelUserBand3: UILabel!
    @IBOutlet weak var LabelUserBand4: UILabel!
    @IBOutlet weak var LabelUserBand5: UILabel!
    
    @IBOutlet weak var TextVigilance1: UITextField!
    @IBOutlet weak var TextVigilance2: UITextField!
    @IBOutlet weak var TextFramesinFeatAveraging: UITextField!
    @IBOutlet weak var TextNewClassCreationTime: UITextField!
    @IBOutlet weak var TextDecisionSmoothingTime: UITextField!
    
    @IBOutlet weak var sliderUserBand1: UISlider!
    @IBOutlet weak var sliderUserBand2: UISlider!
    @IBOutlet weak var sliderUserBand3: UISlider!
    @IBOutlet weak var sliderUserBand4: UISlider!
    @IBOutlet weak var sliderUserBand5: UISlider!
    
    @IBOutlet weak var switchHybrid: UISwitch!
    @IBOutlet weak var switchUserGains: UISwitch!
    
    // local vars
    var refresh: Timer!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem
        
         super.viewDidLoad()
        
        // Only allowed to set if audio not playing
        let notPlaying = !audioController.playAudio
        
        // Noise Classifer Settings
        TextVigilance1.isEnabled = notPlaying
        TextVigilance2.isEnabled = notPlaying
        TextFramesinFeatAveraging.isEnabled = notPlaying
        TextNewClassCreationTime.isEnabled = notPlaying
        TextDecisionSmoothingTime.isEnabled = notPlaying
        
        sliderUserBand1.isEnabled = false
        sliderUserBand2.isEnabled = false
        sliderUserBand3.isEnabled = false
        sliderUserBand4.isEnabled = false
        sliderUserBand5.isEnabled = false
        
        // saving/Loading switch for noise classifier and user band gains
        switchHybrid.isOn         = audioController.hybridClassification
        switchUserGains.isOn = audioController.saveClassification
 
        // User Band Gains sliders
        sliderUserBand1.value = GainMap(audioController.personalizedGainBand1, sliderToGain: false)
        LabelUserBand1.text   = "0-500Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand1))"
        
        sliderUserBand2.value = GainMap(audioController.personalizedGainBand2, sliderToGain: false)
        LabelUserBand2.text   = "500-1000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand2))"
        
        sliderUserBand3.value = GainMap(audioController.personalizedGainBand3, sliderToGain: false)
        LabelUserBand3.text   = "1000-2000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand3))"
        
        sliderUserBand4.value = GainMap(audioController.personalizedGainBand4, sliderToGain: false)
        LabelUserBand4.text   = "2000-4000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand4))"
        
        sliderUserBand5.value = GainMap(audioController.personalizedGainBand5, sliderToGain: false)
        LabelUserBand5.text   = "Above 4000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand5))"
        
        // Show the default values of noise classifier parameters
        TextVigilance1.text = String(format: "%.2f", audioController.vigilanceVal1)
        TextVigilance2.text = String(format: "%.2f", audioController.vigilanceVal2)
        TextFramesinFeatAveraging.text = String (format: "%d", audioController.FeatureAvgSize)
        TextNewClassCreationTime.text = String (format: "%.1f", audioController.NewClassCreationTime)
        TextDecisionSmoothingTime.text = String (format: "%.1f", audioController.DecsionClassTime)
        
        // Recognize taps to exit keyboard
        let tapToExitKeyboard: UITapGestureRecognizer =
            UITapGestureRecognizer(target: self,
                                   action: #selector(self.dismissKeyboard))
        view.addGestureRecognizer(tapToExitKeyboard)

    }

    deinit {
        // Remove the notification
        NotificationCenter.default.removeObserver(self)
    }
    /// A function to show user band gain value.
    ///
    /// - Parameters:
    ///   - value: The value to map.
    ///   - sliderToAmp: Map a slider position to an amplification if true, or map an amplification value to a slider position if false.
    /// - Returns: The mapped value.
    func GainMap(_ value: Float, sliderToGain: Bool) -> Float {
        if sliderToGain {
            return 0.1*value
        }
        else {
            return value
        }
    }
    
    /*
     @IBAction func TextVigilance1EdittingDidEnd(_ sender: UITextField) {
     let Vigilance1: Float = Float(audioController.vigilanceVal1)
     if let text: String = sender.text {
     if let newVigilance1: Float = Float(text) {
     if newVigilance1 < 0 {
     sender.text = String(format: "%.2f", Vigilance1)
     }
     else {
     audioController.update(vigilanceVal1: newVigilance1)
     sender.text = String(format: "%.2f", newVigilance1)
     }
     }
     else {
     sender.text = String(format: "%.2f", Vigilance1)
     }
     }
     else {
     sender.text = String(format: "%.2f", Vigilance1)
     }
     }
 */
    
    @IBAction func TextVigilance1DidEdittingEnd(_ sender: UITextField) {
        let Vigilance1: Float = Float(audioController.vigilanceVal1)
        if let text: String = sender.text {
            if let newVigilance1: Float = Float(text) {
                if newVigilance1 < 0 {
                    sender.text = String(format: "%.2f", Vigilance1)
                }
                else {
                    audioController.update(vigilanceVal1: newVigilance1)
                    sender.text = String(format: "%.2f", newVigilance1)
                }
            }
            else {
                sender.text = String(format: "%.2f", Vigilance1)
            }
        }
        else {
            sender.text = String(format: "%.2f", Vigilance1)
        }
    }
    
    
    @IBAction func TextVigilance2DidEdittingEnd(_ sender: UITextField) {
        let Vigilance2: Float = Float(audioController.vigilanceVal2)
        if let text: String = sender.text {
            if let newVigilance2: Float = Float(text) {
                if newVigilance2 < 0 {
                    sender.text = String(format: "%.2f", Vigilance2)
                }
                else {
                    audioController.update(vigilanceVal2: newVigilance2)
                    sender.text = String(format: "%.2f", newVigilance2)
                }
            }
            else {
                sender.text = String(format: "%.2f", Vigilance2)
            }
        }
        else {
            sender.text = String(format: "%.2f", Vigilance2)
        }
    }
    
    
    @IBAction func TextFeatAvgBuffDidEdittingEnd(_ sender: UITextField) {
        let FeatAvgBuff: Int = Int(audioController.FeatureAvgSize)
        if let text: String = sender.text {
            if let newFeatAvgBuff: Int = Int(text) {
                if newFeatAvgBuff < 0 {
                    sender.text = String(format: "%d", FeatAvgBuff)
                }
                else {
                    audioController.update(FeatureAvgSize: newFeatAvgBuff)
                    sender.text = String(format: "%d", newFeatAvgBuff)
                }
            }
            else {
                sender.text = String(format: "%d", FeatAvgBuff)
            }
        }
        else {
            sender.text = String(format: "%d", FeatAvgBuff)
        }
    }
    
    
    @IBAction func TextNewClassTimeDidEdittingEnd(_ sender: UITextField) {
        let newClassBuff: Float = Float(audioController.NewClassCreationTime)
        if let text: String = sender.text {
            if let newNewClassBuff: Float = Float(text) {
                if newNewClassBuff < 0 {
                    sender.text = String(format: "%.2f", newClassBuff)
                }
                else {
                    audioController.update(NewClassCreationTime: newNewClassBuff)
                    sender.text = String(format: "%.2f", newNewClassBuff)
                }
            }
            else {
                sender.text = String(format: "%.2f", newClassBuff)
            }
        }
        else {
            sender.text = String(format: "%.2f", newClassBuff)
        }
    }
    
    
    @IBAction func TextDecSmoothTimeDidEdittingEnd(_ sender: UITextField) {
        let DecSmoothBuff: Float = Float(audioController.DecsionClassTime)
        if let text: String = sender.text {
            if let newDecSmoothBuff: Float = Float(text) {
                if newDecSmoothBuff < 0 {
                    sender.text = String(format: "%.2f", DecSmoothBuff)
                }
                else {
                    audioController.update(DecsionClassTime: newDecSmoothBuff)
                    sender.text = String(format: "%.2f", newDecSmoothBuff)
                }
            }
            else {
                sender.text = String(format: "%.2f", DecSmoothBuff)
            }
        }
        else {
            sender.text = String(format: "%.2f", DecSmoothBuff)
        }
    }
    
    
    @IBAction func SliderBand1DidEdittingEnd(_ sender: UISlider) {
        audioController.update(personalizedGainBand1 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand1.text   = "0-500Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand1))"
    }
    
    
    @IBAction func SliderBand2DidEdittingEnd(_ sender: UISlider) {
        audioController.update(personalizedGainBand1 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand2.text   = "500-1000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand2))"
    }
    
    @IBAction func SliderBand3DidEdittingEnd(_ sender: UISlider) {
        audioController.update(personalizedGainBand1 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand3.text   = "1000-2000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand3))"
    }
    
    
    @IBAction func SliderBand4DidEdittingEnd(_ sender: UISlider) {
        audioController.update(personalizedGainBand1 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand4.text   = "2000-4000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand4))"
    }
    
    
    @IBAction func SliderBand5DidEdittingEnd(_ sender: UISlider) {
        audioController.update(personalizedGainBand1 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand5.text   = "Above 4000Hz:   \(String(format: "%0.1f", audioController.personalizedGainBand5))"
    }
    
    
    @IBAction func switchHybridValueChanged(_ sender: UISwitch) {
        audioController.update(hybridClassification: sender.isOn)
        // Refresh with animation based on changed state
        sender.isOn = audioController.hybridClassification
    }
    
    @IBAction func switchSavingRepChanged(_ sender: UISwitch) {
        audioController.update(saveClassification: sender.isOn)
        // Refresh with animation based on changed state
        sender.isOn = audioController.saveClassification
    }
    
    /// Dismisses the keyboard when the screen is tapped after editing a field.
    func dismissKeyboard() {
        view.endEditing(true) // End editing on tap
    }

    /// Shows an error alert if an error occurs when entering a number.
    ///
    /// - Parameter intRequired: Whether or not an integer is required in the field.
    func showErrorAlert(intRequired: Bool = false, posRequired:Bool = true) {
        let posstr: String = posRequired ? "positive" : "valid"
        let message: String = intRequired ? "Please enter only \(posstr) integer numbers." : "Please enter only \(posstr) numbers."
        let alert: UIAlertController = UIAlertController(title: "Improper Value Entered",
                                                         message: message,
                                                         preferredStyle: .alert)
        let ok = UIAlertAction(title: "OK", style: .default, handler: {
            UIAlertAction in
        })
        alert.addAction(ok)
        self.present(alert, animated: true, completion: nil)
    }
    
    
    //************************************
    /*override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }*/

    // MARK: - Table view data source

    /*override func numberOfSections(in tableView: UITableView) -> Int {
        // #warning Incomplete implementation, return the number of sections
        return 0
    }*/

    /*override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        // #warning Incomplete implementation, return the number of rows
        return 0
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

}
