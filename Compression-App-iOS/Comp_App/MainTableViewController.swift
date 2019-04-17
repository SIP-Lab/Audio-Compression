//
//  MainTableViewController.swift
//  SPP_Integrated_App
//
//  Created by Akshay Chitale on 6/12/17.
//  Modified by Nasim Alamdari on 07/30/2018
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import UIKit
import Foundation
//var mainTableViewController: MainTableViewController!
/// The view controller for the root view of the app.


class MainTableViewController: UITableViewController {
    
    // Outlets to UI components
    @IBOutlet weak var switchSaveOutput: UISwitch!
    @IBOutlet weak var switchOutputType: UISwitch!
    @IBOutlet weak var switchCompression: UISwitch!
    @IBOutlet weak var labelAmplification: UILabel!
    @IBOutlet weak var sliderAmplification: UISlider!
    @IBOutlet weak var labelStartStop: UILabel!
    @IBOutlet weak var UTDlogoLabName: UILabel!
    @IBOutlet weak var utdLogo: UIImageView!
    @IBOutlet weak var LabelFrameProcTime: UILabel!
    @IBOutlet weak var textFieldCalibration: UITextField!
    @IBOutlet weak var labelSPL: UILabel!

    // local vars
    var refreshVad: Timer!
    var refresh_SPL: Timer!
    var refreshFrameTime: Timer!
    // local vars
    /// The green color for the start button text.
    var ratingText: String!
    let startColor: UIColor = UIColor(red: 56.0/256, green: 214.0/256, blue: 116.0/256, alpha: 1.0) // Same as green on switch
    var path: URL = MainTableViewController.documentsDirectory.appendingPathComponent("Ratings.txt")
    override func viewDidLoad() {
        super.viewDidLoad()

        
        //let bounds = self.navigationController!.navigationBar.bounds
        let myView: UIView = UIView()
        let title: UILabel = UILabel(frame: CGRect(x:-145, y:-10, width:300, height:30))
        title.textAlignment = .center;
        title.font = UIFont.boldSystemFont(ofSize: title.font.pointSize);
        title.text = "Compression App"//"Signal and Image Processing Lab"

        let image: UIImage = UIImage(named: "utd_logo.png")!
       //let myImageView: UIImageView = UIImageView(image: image)
       //myImageView.frame = CGRect(x:-170, y:-10, width:30, height:30)
       // myImageView.align
       // let height: CGFloat = 50 //whatever height you want
        
        //self.navigationController!.navigationBar.frame = CGRect(x: 0, y: 0, width: bounds.width, height: height)
        myView.addSubview(title)
        //myView.addSubview(myImageView)
        
        UTDlogoLabName.text = "Signal and Image Processing Lab"
        utdLogo.image =  image;
        
        self.navigationItem.titleView = myView

        //self.navigationItem.titleView.pos
        
        ratingText = ""

        tableView.beginUpdates()
        tableView.endUpdates()
        
        // Output Type switch
        switchCompression.isOn = audioController.compressionOutputType;
        switchCompression.isEnabled = audioController.playAudio
        
        
        // Amplification slider
        sliderAmplification.value = ampMap(audioController.amplification, sliderToAmp: false)
        sliderAmplification.isEnabled = false
        labelAmplification.text = "Amplification: \(String(format: "%0.2f", audioController.amplification))x"
        
        
        // Calibration and SPL
        textFieldCalibration.text = String(format: "%.0f", audioController.calibration)
        

        // Timer to refresh SPL and Process time
        refreshProcTime()
        refreshFrameTime = Timer.scheduledTimer(timeInterval: TimeInterval(audioController.dbUpdateInterval),
                                       target: self,
                                       selector: #selector(self.refreshProcTime),
                                       userInfo: nil,
                                       repeats: true)

        
        // Set start/stop colors
        startStop(start: audioController.playAudio, setMic: true) // Show stopped or started
  
       
        let longPressToExitKeyboard: UILongPressGestureRecognizer =
            UILongPressGestureRecognizer(target: self,
                                   action: #selector(self.dismissKeyboard))
        view.addGestureRecognizer(longPressToExitKeyboard)
 

    }
    
    func dismissKeyboard() {
        view.endEditing(true) // End editing on tap
    }
    

    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        // to update the switches if any .json file is selected:
        //switchOutputType.isOn = audioController.noiseReductionOutputType;
        switchCompression.isOn = audioController.compressionOutputType;
        //switchAFC.isOn = audioController.activeAFC;
    

    }
    
    /// Refreshes the frame processing time (ms) and audio level.
    func refreshProcTime() {
        LabelFrameProcTime.text = "\(String(format:"%.2f", audioController.processTime)) ms"
        labelSPL.text = "\(String(format:"%.0f", audioController.dbpower)) dB SPL"

    }
    
    
    deinit {
        // Remove the notification
        NotificationCenter.default.removeObserver(self)
    }
    
    static var documentsDirectory: URL {
        get {
            let paths = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)
            let documentsDirectory = paths[0]
            return documentsDirectory
        }
    }
    
    
    func showErrorAlert(_ message: String) {
        let alert: UIAlertController = UIAlertController(title: "Save File Error",
                                                         message: message,
                                                         preferredStyle: .alert)
        let ok = UIAlertAction(title: "OK", style: .default, handler: {
            UIAlertAction in
        })
        alert.addAction(ok)
        self.present(alert, animated: true, completion: nil)
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
                //showErrorAlert()
            }
        }
        else {
            sender.text = String(format: "%.0f", calibration)
            //showErrorAlert()
        }
    }

    @IBAction func saveOutputinFile(_ sender: UISwitch) {
        audioController.update(doSaveFile: sender.isOn)
    }
    
    
    

    @IBAction func compressionOutputTypeValueChanded(_ sender: UISwitch) {
        audioController.update(compressionOutputType: sender.isOn)
        if sender.isOn {
            labelAmplification.text = "Amplification: \(String(format: "%0.2f", audioController.amplification))x"
            sliderAmplification.value = ampMap(audioController.amplification, sliderToAmp: false)
        }
    }
    // Amplification slider
    @IBAction func sliderAmplificationValueChanged(_ sender: UISlider) {
        audioController.update(amplification: ampMap(sender.value, sliderToAmp: true))
        labelAmplification.text = "Amplification: \(String(format: "%0.2f", audioController.amplification))x"
    }
    
    //Quiet Adjustment slider update
    let step: Float = 1
    
    /// Starts or stops the audio.
    ///
    /// - Parameters:
    ///   - start: Starts the audio if true, or stops the audio if false.
    ///   - setMic: Enables the microphone input on start if true.
    func startStop(start: Bool, setMic: Bool) {
        // setMic true for playing back input, false for playing audio from file
        if start {
            labelStartStop.text = "STOP"
            labelStartStop.textColor = UIColor.red
            audioController.update(micStatus: setMic)
            audioController.start()
            // Set button enables
            switchCompression.isEnabled = true
            switchSaveOutput.isEnabled = false
            sliderAmplification.isEnabled = true

        }
        else {
            labelStartStop.text = "LIVE AUDIO"
            labelStartStop.textColor = startColor
            audioController.stop()
            audioController.update(micStatus: false)
            // Set button enables
            switchCompression.isEnabled = false
            switchSaveOutput.isEnabled = true
            sliderAmplification.isEnabled = false
        
        }
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
        
    /// A function to map slider value to amplification value.
    ///
    /// - Parameters:
    ///   - value: The value to map.
    ///   - sliderToAmp: Map a slider position to an amplification if true, or map an amplification value to a slider position if false.
    /// - Returns: The mapped value.
    func ampMap(_ value: Float, sliderToAmp: Bool) -> Float {
        if sliderToAmp {
            return value * value
        }
        else {
            return sqrtf(value)
        }
    }

    
    // Hide and show rows
    override func tableView(_ tableView: UITableView, estimatedHeightForRowAt indexPath: IndexPath) -> CGFloat {
        return UITableViewAutomaticDimension
    }
    
    // Selecting row which functions as a button
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if indexPath.section == 4 && indexPath.row == 0 {
            startStop(start: !audioController.playAudio, setMic: true) // If it's playing, stop it; if not, start it
            tableView.deselectRow(at: indexPath, animated: true)
        }
    }
    
}
