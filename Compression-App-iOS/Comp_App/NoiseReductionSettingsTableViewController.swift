//
//  NoiseReductionSettingsTableViewController.swift
//  SPP_Integrated_App
//
//  Created by Akshay Chitale on 6/29/17.
//  Modified by Tahsin Ahmed Chowdhury on 10/15/17
//  Modified by Nasim Alamdari on 07/30/2018
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import UIKit
import Foundation

/// The view controller for setting the advanced settings.
class NoiseReductionSettingsTableViewController: UITableViewController {
    
    // Outlets to UI Components
    @IBOutlet weak var labelSamplingFrequency: UILabel!
    @IBOutlet weak var labelWindowSize: UILabel!
    @IBOutlet weak var labelStepSize: UILabel!
    @IBOutlet weak var textFieldCalibration: UITextField!
    @IBOutlet weak var labelSBL: UILabel!
    @IBOutlet weak var labelCurrentGains: UILabel!
    @IBOutlet weak var textFieldUpdateInterval: UITextField!
    
    @IBOutlet weak var textFieldNoiseEstimateTime: UITextField!
    
    
    @IBOutlet weak var TextVigilance1: UITextField!
    @IBOutlet weak var TextVigilance2: UITextField!
    @IBOutlet weak var TextFramesinFeatAveraging: UITextField!
    @IBOutlet weak var TextNewClassCreationTime: UITextField!
    @IBOutlet weak var TextDecisionSmoothingTime: UITextField!
    @IBOutlet weak var TextGainAvgTime: UITextField!
    
    @IBOutlet weak var LabelUserBand1: UILabel!
    @IBOutlet weak var LabelUserBand2: UILabel!
    @IBOutlet weak var LabelUserBand3: UILabel!
    @IBOutlet weak var LabelUserBand4: UILabel!
    @IBOutlet weak var LabelUserBand5: UILabel!
    
    @IBOutlet weak var sliderUserBand1: UISlider!
    @IBOutlet weak var sliderUserBand2: UISlider!
    @IBOutlet weak var sliderUserBand3: UISlider!
    @IBOutlet weak var sliderUserBand4: UISlider!
    @IBOutlet weak var sliderUserBand5: UISlider!
    
    @IBOutlet weak var switchHybrid: UISwitch!
    @IBOutlet weak var switchUserGains: UISwitch!

    
    // local vars
    var refresh: Timer!
    /// The maximum value of the gains sliders
    let SLIDER_MAX: Float = 2.0
    /// The minimum value of the gains sliders.
    let SLIDER_MIN: Float = 0.0
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Only allowed to set if audio not playing
        let notPlaying = !audioController.playAudio

        textFieldUpdateInterval.isEnabled = notPlaying
        textFieldNoiseEstimateTime.isEnabled = notPlaying
        // Sampling Frequency
        let samplingFreq: Int = audioController.fs
        labelSamplingFrequency.text = "\(samplingFreq) Hz"
        
        // Frame (Window) size
        let frame: Float = 2.0 * Float(audioController.stepSize)/Float(audioController.fs)
        labelWindowSize.text = String(format: "%.2f", 1000.0 * frame)
        
        // Step size
        let step: Float = Float(audioController.stepSize)/Float(audioController.fs)
        labelStepSize.text = String(format: "%.2f", 1000.0 * step)
        
        // Calibration and SPL
        textFieldCalibration.text = String(format: "%.0f", audioController.calibration)
        refreshSPL()
        // Timer to refresh SPL
        refresh = Timer.scheduledTimer(timeInterval: TimeInterval(audioController.dbUpdateInterval),
                                       target: self,
                                       selector: #selector(self.refreshSPL),
                                       userInfo: nil,
                                       repeats: true)
        
        // Show update interval
        textFieldUpdateInterval.text = String(format: "%.2f", audioController.dbUpdateInterval)
        textFieldNoiseEstimateTime.text = String(format: "%.2f", audioController.noiseEstimateTime)
        
        
        
        // Noise Classifer Settings
        TextVigilance1.isEnabled = notPlaying
        TextVigilance2.isEnabled = notPlaying
        TextFramesinFeatAveraging.isEnabled = notPlaying
        TextNewClassCreationTime.isEnabled = notPlaying
        TextDecisionSmoothingTime.isEnabled = notPlaying
        TextGainAvgTime.isEnabled = notPlaying
        
        sliderUserBand1.isEnabled = true
        sliderUserBand2.isEnabled = true
        sliderUserBand3.isEnabled = true
        sliderUserBand4.isEnabled = true
        sliderUserBand5.isEnabled = true
        
        // saving/Loading switch for noise classifier and user band gains
        switchHybrid.isOn         = audioController.hybridClassification
        switchUserGains.isOn = audioController.saveClassification
        
        // User Band Gains sliders
        sliderUserBand1.value = GainMap(audioController.personalizedGainBand1, sliderToGain: false)
        LabelUserBand1.text   = "0-500Hz           : \(String(format: "%0.1f", audioController.personalizedGainBand1))"
        
        sliderUserBand2.value = GainMap(audioController.personalizedGainBand2, sliderToGain: false)
        LabelUserBand2.text   = "500-1000Hz     : \(String(format: "%0.1f", audioController.personalizedGainBand2))"
        
        sliderUserBand3.value = GainMap(audioController.personalizedGainBand3, sliderToGain: false)
        LabelUserBand3.text   = "1000-2000Hz   : \(String(format: "%0.1f", audioController.personalizedGainBand3))"
        
        sliderUserBand4.value = GainMap(audioController.personalizedGainBand4, sliderToGain: false)
        LabelUserBand4.text   = "2000-4000Hz  : \(String(format: "%0.1f", audioController.personalizedGainBand4))"
        
        sliderUserBand5.value = GainMap(audioController.personalizedGainBand5, sliderToGain: false)
        LabelUserBand5.text   = "Above 4000Hz : \(String(format: "%0.1f", audioController.personalizedGainBand5))"
        
        // Show the default values of noise classifier parameters
        TextVigilance1.text = String(format: "%.2f", audioController.vigilanceVal1)
        TextVigilance2.text = String(format: "%.2f", audioController.vigilanceVal2)
        TextFramesinFeatAveraging.text = String (format: "%d", audioController.FeatureAvgSize)
        TextNewClassCreationTime.text = String (format: "%.1f", audioController.NewClassCreationTime)
        TextDecisionSmoothingTime.text = String (format: "%.1f", audioController.DecsionClassTime)
        TextGainAvgTime.text = String (format: "%.2f", audioController.gainAvgTime)
        
   
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
    
    func GainMap(_ value: Float, sliderToGain: Bool) -> Float {
        if sliderToGain {
            return value/10 // this is for update
        }
        else {
            return value*10 // this is for the GUI displayment
        }
    }
    
    
    /* deprecated
    @IBAction func textFieldWindowSizeEditingDidEnd(_ sender: UITextField) {
        let frame: Float = Float(audioController.frameSize)/Float(audioController.fs)
        if let text: String = sender.text {
            if let newWindowSize: Float = Float(text) {
                if newWindowSize <= 0 || newWindowSize != round(newWindowSize) {
                    sender.text = String(format: "%.2f", 1000.0 * frame)
                    showErrorAlert(intRequired: true)
                }
                audioController.update(frameSize: Float(newWindowSize))
                sender.text = String(format: "%.2f", newWindowSize)
            }
            else {
                sender.text = String(format: "%.2f", 1000.0 * frame)
                showErrorAlert(intRequired: true)
            }
        }
        else {
            sender.text = String(format: "%.2f", 1000.0 * frame)
            showErrorAlert(intRequired: true)
        }
    }
     */
    
    @IBAction func textFieldNoiseEstimateTimeEditDidChanged(_ sender: UITextField) {
        let noiseEstimateTime = audioController.noiseEstimateTime
        if let text: String = sender.text {
            if let newEstimateTime: Float = Float(text) {
                if newEstimateTime < 0 {
                    sender.text = String(format: "%.2f", noiseEstimateTime)
                    showErrorAlert()
                }
                else {
                    audioController.update(noiseEstimateTime: newEstimateTime)
                    sender.text = String(format: "%.2f", newEstimateTime)
                }
            }
            else {
                sender.text = String(format: "%.2f", noiseEstimateTime)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.2f", noiseEstimateTime)
            showErrorAlert()
        }
    }
    
    @IBAction func textFieldCalibrationEditingDidEnd(_ sender: UITextField) {
        let calibration: Float = audioController.calibration
        if let text: String = sender.text {
            if let newCalibration: Float = Float(text) {
                audioController.update(calibration: newCalibration)
                sender.text = String(format: "%.0f", newCalibration)
            }
            else {
                sender.text = String(format: "%.0f", calibration)
                showErrorAlert(posRequired: false)
            }
        }
        else {
            sender.text = String(format: "%.0f", calibration)
            showErrorAlert(posRequired: false)
        }
    }
    
    @IBAction func textFieldUpdateIntervalEditingDidEnd(_ sender: UITextField) {
        let dbUpdateInterval = audioController.dbUpdateInterval
        if let text: String = sender.text {
            if let newUpdateInterval: Float = Float(text) {
                if newUpdateInterval < 0 {
                    sender.text = String(format: "%.2f", dbUpdateInterval)
                    showErrorAlert()
                }
                else {
                    audioController.update(dbUpdateInterval: newUpdateInterval)
                    sender.text = String(format: "%.2f", newUpdateInterval)
                }
            }
            else {
                sender.text = String(format: "%.2f", dbUpdateInterval)
                showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.2f", dbUpdateInterval)
            showErrorAlert()
        }
    }
    
    
    @IBAction func TextGainAvgDidEdditingEnd(_ sender: UITextField) {
        let gainAvgTime: Float = Float(audioController.gainAvgTime)
        if let text: String = sender.text {
            if let newGainAvg: Float = Float(text) {
                if newGainAvg < 0 {
                    sender.text = String(format: "%.2f", gainAvgTime)
                }
                else {
                    audioController.update(gainAvgTime: newGainAvg)
                    sender.text = String(format: "%.2f", newGainAvg)
                }
            }
            else {
                sender.text = String(format: "%.2f", gainAvgTime)
            }
        }
        else {
            sender.text = String(format: "%.2f", gainAvgTime)
        }
    }
    
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
    
    let step: Float = 1;
    @IBAction func SliderBand1DidEdittingEnd(_ sender: UISlider) {
        let roundedValue1 = round(sender.value / step) * step
        sender.value = roundedValue1
        audioController.update(personalizedGainBand1 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand1.text   = "0-500Hz           : \(String(format: "%0.1f", audioController.personalizedGainBand1))"
    }
    
    @IBAction func SliderBand2DidEdittingEnd(_ sender: UISlider) {
        let roundedValue2 = round(sender.value / step) * step
        sender.value = roundedValue2
        audioController.update(personalizedGainBand2 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand2.text   = "500-1000Hz     : \(String(format: "%0.1f", audioController.personalizedGainBand2))"
    }
    
    @IBAction func SliderBand3DidEdittingEnd(_ sender: UISlider) {
        let roundedValue3 = round(sender.value / step) * step
        sender.value = roundedValue3
        audioController.update(personalizedGainBand3 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand3.text   = "1000-2000Hz   : \(String(format: "%0.1f", audioController.personalizedGainBand3))"
    }
    
    @IBAction func SliderBand4DidEdittingEnd(_ sender: UISlider) {
        let roundedValue4 = round(sender.value / step) * step
        sender.value = roundedValue4
        audioController.update(personalizedGainBand4 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand4.text   = "2000-4000Hz  : \(String(format: "%0.1f", audioController.personalizedGainBand4))"
    }
    
    @IBAction func SliderBand5DidEdittingEnd(_ sender: UISlider) {
        let roundedValue5 = round(sender.value / step) * step
        sender.value = roundedValue5
        audioController.update(personalizedGainBand5 : GainMap(sender.value, sliderToGain: true))
        LabelUserBand5.text   = "Above 4000Hz : \(String(format: "%0.1f", audioController.personalizedGainBand5))"
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
    
    /// Refreshes the values of the audio level.
    func refreshSPL() {
        labelSBL.text = "\(String(format:"%.0f", audioController.dbpower)) dB SPL"
        //labelCurrentGains.text = "\(String(format:"%.0f", audioController.processTime)) ms"
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
    
  /*override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let segueIdentifier: String
         if indexPath.section == 3 && indexPath.row == 0 {
            segueIdentifier = "showView4"
            self.performSegue(withIdentifier: segueIdentifier, sender: self)
        }
    
    }*/
}
