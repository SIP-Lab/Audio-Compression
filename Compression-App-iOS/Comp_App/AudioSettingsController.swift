//
//  AudioSettingsController.swift
//  SPP_Integrated_App
//
//  Created by Akshay Chitale on 6/25/17.
//  Modified by Nasim Alamdari on 07/30/2018
//  Copyright © 2017 UT Dallas. All rights reserved.
//

import Foundation


/// Globally availible audio settings, set in `AppDelegate.swift` on app launch and for use in all view controllers.
var audioController: AudioSettingsController!

/// A controller for starting and stopping the audio, as well as changing the audio settings.
///
/// This class serves as an interface for Swift code to interact with the underlying Objective-C and C audio code.
class AudioSettingsController {
    // Later have private var for IosAudioController, for now is global
    
    /// Keeps track of "Low", "Medium", and "High" gains. This variable can be used to store other gains arrays as well.
    ///
    /// The element at `self.currentGains` is the current gains.
    private var gainsTable: Dictionary<String, [Float]>
    
    // for compression settings from .json file
    private var compressionTable: Dictionary<String, [Float]>
    
    
    /// The current array of gains.
    public private(set) var currentGains: String { // Private setter, public getter
        get {
            if(self.autoGains) {
                // Must be "Low", "Medium". or "High"
                switch(iosAudio.settings.pointee.currentGains) {
                case 0:
                    return "Low"
                case 1:
                    return "Medium"
                case 2:
                    return "High"
                default:
                    return self._currentGains
                }
            }
            else {
                return self._currentGains
            }
        }
        set {
            self._currentGains = newValue
        }
    }
    private var _currentGains: String;
    
    
    /// The default array of compression parameters.
    let defaultCompressionVal: [Float] = [1, 1, 1, 1]
    
    
    /// The default array of gains, where every gain is 1.0.
    let defaultGains: [Float] = [1, 1, 1, 1, 1]
    
    
    /// The definition of the "Low" audio level, in dB SPL.
    /*var lowDef: Int {
        return Int(iosAudio.settings.pointee.lowDef);
    }
    
    /// The definition of the "Medium" audio level, in dB SPL.
    var mediumDef: Int {
        return Int(iosAudio.settings.pointee.mediumDef);
    }
    
    /// The definition of the "High" audio level, in dB SPL.
    var highDef: Int {
        return Int(iosAudio.settings.pointee.highDef);
    }*/
    
    /// The name of the file from which the current gains were read.
    ///
    /// This variable is for public use. This class does not read or mutate this variable.
    var gainsFile: String = "Default"
    

    /// Initializes the `AudioSettingsController` with "Low", "Medium", and "High" gains and default settings.
    init() {
        iosAudio = IosAudioController()
        gainsTable = Dictionary<String, [Float]>()
        gainsTable["Low"] = defaultGains
        gainsTable["Medium"] = defaultGains
        gainsTable["High"] = defaultGains
        self._currentGains = "Medium"

        compressionTable = Dictionary<String, [Float]>() //initializing the compressionTable
        compressionTable["Band1"] = defaultCompressionVal
        compressionTable["Band2"] = defaultCompressionVal
        compressionTable["Band3"] = defaultCompressionVal
        compressionTable["Band4"] = defaultCompressionVal
        compressionTable["Band5"] = defaultCompressionVal
        
        setGains()
        
    }
    
    /// Initializes the `AudioSettingsController` with an array of current gains.
    ///
    /// This init first calls the default constructor, which initializes the "Low", "Medium", and "High" gains to `self.defaultGains`.
    ///
    /// - Parameters:
    ///   - gains: The array of gains to set, of length `self.numGains`.
    ///   - currentGains: The name of the array of gains to set.
    convenience init(gains: [Float], currentGains: String) {
        self.init()
        let set: Bool = update(gains: gains, currentGains: currentGains, setOnly: false)
        if !set {
            print("Default gains set")
        }
    }
    deinit {
        // Deallocate memory
        iosAudio.destructSettings()
    }
    

    /// Sets the gains in `iosAudio` to the current gains.
    private func setGains() {
        // Is a mutable pointer, but we know that Obj-C and C code only get gains
        iosAudio.settings.pointee.lowGains      = UnsafeMutablePointer<Float>(mutating: gainsTable["Low"]!)
        iosAudio.settings.pointee.mediumGains   = UnsafeMutablePointer<Float>(mutating: gainsTable["Medium"]!)
        iosAudio.settings.pointee.highGains     = UnsafeMutablePointer<Float>(mutating: gainsTable["High"]!)
        iosAudio.settings.pointee.gains         = UnsafeMutablePointer<Float>(mutating: gainsTable[currentGains]!)
    }
    
    /// The number of gains an array of gains must have.
    var numGains: Int {
        return Int(iosAudio.settings.pointee.numGains)
    }
    
    /// The current array of gains in use, of length `self.numGains`.
    var gains: [Float] {
        // Gains from here are set only, because AudioSettingsController stores more info than struct Settings
        // Update functions and constructors guarantee gains exist
        return gainsTable[currentGains]!
    }
    
    /// The current "Low", "Medium", and "High" gains arrays.
    /// - "Low" gains at index 0.
    /// - "Medium" gains at index 1.
    /// - "High" gains at index 2.
    var LMHGains: [[Float]] {
        // LMH known to exist by constructor, return list of gains lists
        return [gainsTable["Low"]!, gainsTable["Medium"]!, gainsTable["High"]!]
    }
    
    /// Whether the gains are being automatically selected. If false, gains are manually selected with `self.currentGains`.
    var autoGains: Bool {
        return iosAudio.settings.pointee.autoGains != 0 ? true : false
    }

    // ************** Test only End initi **************************************************************
    
    /// Starts the audio.
    ///
    /// This function is safe to use whether or not the audio is currently stopped.
    func start() {
        if !playAudio {
            iosAudio.start()
            iosAudio.settings.pointee.playAudio = 1
        }
    }
    
    /// Stops the audio.
    ///
    /// This function is safe to use whether or not the audio is currently playing.
    func stop() {
        if playAudio {
            iosAudio.stop()
            iosAudio.settings.pointee.playAudio = 0
        }
    }
    
    
    /// The number of gains an array of gains must have.
//    var numGains: Int {
//        return Int(iosAudio.settings.pointee.numGains)
//    }
    
    /// The current sampling frequency.
    var fs: Int {
        return Int(iosAudio.settings.pointee.fs)
    }
    
    /// The current frame size, a.k.a. the window size.
    var frameSize: Int {
        return Int(iosAudio.settings.pointee.frameSize)
    }
    
    /// The current step size, a.k.a. the overlap size.
    var stepSize: Int {
        return Int(iosAudio.settings.pointee.stepSize)
    }
    
    /// The current microphone status. Is using audio input from the microphone if true, or from a file if false.
    var micStatus: Bool {
        return iosAudio.settings.pointee.micStatus != 0 ? true : false
    }
    
    /// Whether audio is currently playing.
    var playAudio: Bool {
        return iosAudio.settings.pointee.playAudio != 0 ? true : false
    }
    

    
    var compressionOutputType: Bool {
        return iosAudio.settings.pointee.compressionOutputType != 0 ? true : false
    }
    
    
    /// The current amplification of the processed audio output.
    var amplification: Float {
        return Float(iosAudio.settings.pointee.amplification)
    }
    
    /// Whether the gains are being automatically selected. If false, gains are manually selected with `self.currentGains`.
//    var autoGains: Bool {
//        return iosAudio.settings.pointee.autoGains != 0 ? true : false
//    }
    
    /// The audio level, in dB SPL.
    var dbpower: Float {
        return iosAudio.settings.pointee.dbpower
    }
    
    var quietVal: Float {
        return iosAudio.settings.pointee.quiet
    }
    
    var processTime: Float {
        return iosAudio.settings.pointee.processTime
    }
    
    /// The calibration constant for audio level measuring, in dB.
    var calibration: Float {
        return iosAudio.settings.pointee.calibration
    }
    
    /// The interval to wait before updating the audio level, in seconds.
    var dbUpdateInterval: Float {
        return iosAudio.settings.pointee.dbUpdateInterval
    }

    
    /// the gain averagin time for wiener noise reduction, in seconds.
    var gainAvgTime: Float {
        return iosAudio.settings.pointee.averagedGainsTime
    }

    //*****************************************************************
    

    

    /// Updates the sampling frequency.
    ///
    /// - Parameter fs: The new sampling frequency, in Hertz.
    func update(fs: Int) {
        iosAudio.settings.pointee.fs = Int32(fs)
    }
    
    /// Updates the frame size, a.k.a. the window size.
    ///
    /// - Parameter frameSize: The new frame size, in milliseconds.
    func update(frameSize: Float) {
        iosAudio.settings.pointee.frameSize = Int32(frameSize * Float(fs) / 1000.0)
    }
    
    /// Updates the step size, a.k.a. the overlap size.
    ///
    /// - Parameter stepSize: The new step size, in milliseconds.
    func update(stepSize: Float) {
        iosAudio.settings.pointee.stepSize = Int32(stepSize * Float(fs) / 1000.0)
    }
    func update(classLabel: Int) {
        iosAudio.settings.pointee.classLabel = Int32(classLabel)
    }
    /// Updates the microphone status.
    ///
    /// - Parameter micStatus: If true, use microphone audio input. If false, use audio input from file.
    func update(micStatus: Bool) {
        iosAudio.settings.pointee.micStatus = Int32(micStatus ? 1 : 0)
    }
    
    
    func update(compressionOutputType: Bool) {
        iosAudio.settings.pointee.compressionOutputType = Int32(compressionOutputType ? 1 : 0)
    }
    
    func update(doSaveFile: Bool) {
        iosAudio.settings.pointee.doSaveFile = Int32(doSaveFile ? 1 : 0)
    }
    
    // Update quiet value
    func update(quietVal: Float){
        iosAudio.settings.pointee.quiet = Float(quietVal)
    }
    
    
    /// Updates the calibration constant for audio level calculation
    ///
    /// - Parameter calibration: The new calibration constant for audio level measuring, in dB.
    func update(calibration: Float) {
        iosAudio.settings.pointee.calibration = calibration
    }
    
    /// Updates the interval to wait before updating the audio level.
    ///
    /// - Parameter dbUpdateInterval: The new interval to wait, in seconds.
    func update(dbUpdateInterval: Float) {
        iosAudio.settings.pointee.dbUpdateInterval = dbUpdateInterval
    }

    
    /// Update the gain averagin time for wiener noise reduction, in seconds.
    func update (gainAvgTime: Float) {
        iosAudio.settings.pointee.averagedGainsTime = Float(gainAvgTime)
    }
    
    
    
    //*****************************************************************
    

    ///
    /// - Parameter alpha: The new weight of `self.lowDef` in the boundary calculation.
    func update(alpha: Float) {
        iosAudio.settings.pointee.alpha = alpha
    }

    /// - Parameter beta: The new weight of `self.mediumDef` in the boundary calculation.
    func update(beta: Float) {
        iosAudio.settings.pointee.beta = beta
    }

    /// Updates the amplification.
    ///
    /// - Parameter amplification: The new amplification value.
    func update(amplification: Float) {
        iosAudio.settings.pointee.amplification = amplification
    }
    
    // **************** For reading json file start update ***********************************
    
    /// Updates the current array of gains.
    ///
    /// - Parameter gains: The new array of gains, of length `self.numGains`.
    /// - Returns: Whether the update was successful.
    func update(gains: [Float]) -> Bool {
        // Make sure gains array has correct number of gains
        if gains.count != numGains {
            return false
        }
        gainsTable[currentGains]! = gains
        setGains()
        return true
    }
    
    /// Updates whether the gains are automatically selected
    ///
    /// - Parameter autoGains: If true, automatically select the gains based on audio level. If false, manually select the gains.
    func update(autoGains: Bool) {
        iosAudio.settings.pointee.autoGains = Int32(autoGains ? 1 : 0)
    }
    
    /// Updates which array of gains to use.
    ///
    /// - Parameter currentGains: The name of the array of gains to be used.
    /// - Returns: Whether the update was successful.
    func update(currentGains: String) -> Bool {
        // Make sure requested gains table exists
        if gainsTable[currentGains] == nil {
            return false
        }
        self.currentGains = currentGains
        setGains()
        return true
    }
    
    /// Updates a specific array of gains.
    ///
    /// - Parameters:
    ///   - gains: The new array of gains, of length `self.numGains`.
    ///   - currentGains: The name of the array of gains to update.
    ///   - setOnly: A flag for whether to set gains without using them as the current gains after the update.
    /// - Returns: Whether the update was successful.
    func update(gains: [Float], currentGains: String, setOnly: Bool) -> Bool {
        if gains.count != numGains {
            return false
        }
        if !setOnly {
            self.currentGains = currentGains // only change value of gains if setOnly is true; otherwise do not change currentGains
        }
        gainsTable[currentGains] = gains
        setGains()
        return true
    }
    
    /// Resets gains to `self.defaultGains`.
    ///
    /// - Parameter all: If true, resets "Low", "Medium", and "High" gains to default instead of only resetting the current gains.
    func setDefault(all: Bool) {
        if all {
            // Set all gains to default
            gainsTable["Low"] = defaultGains
            gainsTable["Medium"] = defaultGains
            gainsTable["High"] = defaultGains
        }
        else {
            // Set only current gains to default
            gainsTable[currentGains] = defaultGains
        }
        setGains()
    }
    
    /// Updates the "Low", "Medium", and "High" gains from a JSON Object
    ///
    /// The JSON object must have an integer attribute "numGains" that is equal to `self.numGains`, as well as a "gains" object attribute that contains "Low", "Medium", and "High" floating point array attributes of length `self.numGains`
    ///
    /// - Parameters:
    ///   - fromJSON: The JSON object from which to get the update information
    ///   - errorString: An input string in which to store error information should one occur.
    /// - Returns: Whether the update was successful.
    func update(fromJSON: [String: Any], errorString: inout String) -> Bool {
        errorString = ""
        var success: Bool = false;
        // errorString if action fails followed by action
        // success only set to true if all if statements succeed
                   
        // ******* Checking the switch options **********
        errorString = "Required attribute \"compressionSwitch\" not provided"
        if let compressionSwitch = fromJSON["compressionSwitch"]{
            
        errorString = "Required attribute \"compressionSwitch\" is not an integer"
        if let switchComp = compressionSwitch as? Int32{
                
        /*errorString = "Required attribute \"noiseReductionSwitch\" not provided"
        if let noiseReductionSwitch = fromJSON["noiseReductionSwitch"]{
                                                                                
        errorString = "Required attribute \"noiseReductionSwitch\" is not an integer"
        if let switchNoiseRedInt = noiseReductionSwitch as? Int32 {
                                                                                            
        errorString = "Required attribute \"AFCSwitch\" not provided"
        if let AFCSwitch = fromJSON["AFCswitch"]{
                                                                                                
        errorString = "Required attribute \"AFCSwitch\" is not an integer"
         if let switchAFC = AFCSwitch as? Int32{*/
                                                                                                    
        //***** Getting compression settings *****
        errorString = "Required attribute \"CompNumParams\" not provided"
        if let BandNumParams = fromJSON["CompNumParams"] {
                                                                                                        
        errorString = "Required attribute \"CompNumParams\" is not an integer"
        if let BandNumParamsInt = BandNumParams as? Int {
                                                                                                            
        errorString = "Number of each compression band parameters is not equal to \(4)"
        if BandNumParamsInt == 5 { // later on , we will ignore compression threshold 1 at 55 dB SPL, so only 4 parameters
                                                                                                                
        errorString = "Required attribute \"compressionSettings\" not provided"
        if let compSettings = fromJSON["compressionSettings"] {
                                                                                                                    
        errorString = "Required attribute \"compSettings\" is not a dictionary of lists"
        if let compDict = compSettings as? [String: Any] {
                                                                                                                        
        errorString = "Required compSettings \"Band1\" not provided"
        if let band1 = compDict["Band1"] {
                                                                                                                            
        errorString = "Required compSettings \"Band1\" is not a list"
        if let band1List = band1 as? [Any] {
                                                                                                                                
        errorString = "Required compSettings \"Band1\" is not a list of floats"
        if let band1ListFloat = band1List as? [Float] { // this value is going to used for update
                                                                                                                                    
        errorString = "Requied compSettings \"Band1\" does not contain \(BandNumParamsInt) gains"
        if band1ListFloat.count == BandNumParamsInt { // final check for low
                                                                                                                                        
        errorString = "Required compSettings \"Band2\" not provided"
        if let band2 = compDict["Band2"] {
                                                                                                                                            
        errorString = "Required compSettings \"Band2\" is not a list"
        if let band2List = band2 as? [Any] {
                                                                                                                                                
        errorString = "Required compSettings \"Band2\" is not a list of floats"
        if let band2ListFloat = band2List as? [Float] { // this value is going to used for update
                                                                                                                                                    
        errorString = "Requied compSettings \"Band2\" does not contain \(BandNumParamsInt) gains"
        if band2ListFloat.count == BandNumParamsInt { // final check for low
                                                                                                
        errorString = "Required compSettings \"Band3\" not provided"
        if let band3 = compDict["Band3"] {
                                                                                                    
        errorString = "Required compSettings \"Band3\" is not a list"
        if let band3List = band3 as? [Any] {
                                                                                                        
        errorString = "Required compSettings \"Band3\" is not a list of floats"
        if let band3ListFloat = band3List as? [Float] { // this value is going to used for update
                                                                                                            
        errorString = "Requied compSettings \"Band3\" does not contain \(BandNumParamsInt) gains"
        if band3ListFloat.count == BandNumParamsInt { // final check for low
                                                                                                
        errorString = "Required compSettings \"Band4\" not provided"
        if let band4 = compDict["Band4"] {
                                                                                                    
        errorString = "Required compSettings \"Band4\" is not a list"
        if let band4List = band4 as? [Any] {
                                                                                                        
        errorString = "Required compSettings \"Band4\" is not a list of floats"
        if let band4ListFloat = band4List as? [Float] { // this value is going to used for update
                                                                                                            
        errorString = "Requied compSettings \"Band4\" does not contain \(BandNumParamsInt) gains"
        if band4ListFloat.count == BandNumParamsInt { // final check for low
                                                                                                
        errorString = "Required compSettings \"Band5\" not provided"
        if let band5 = compDict["Band5"] {
                                                                                                    
        errorString = "Required compSettings \"Band5\" is not a list"
        if let band5List = band5 as? [Any] {
                                                                                                        
        errorString = "Required compSettings \"Band5\" is not a list of floats"
        if let band5ListFloat = band5List as? [Float] { // this value is going to used for update
                                                                                                            
        errorString = "Requied compSettings \"Band5\" does not contain \(BandNumParamsInt) gains"
        if band5ListFloat.count == BandNumParamsInt { // final check for low
            
        errorString = "Required compSettings \"Band6\" not provided"
        if let band6 = compDict["Band6"] {
                
        errorString = "Required compSettings \"Band6\" is not a list"
        if let band6List = band6 as? [Any] {
                    
        errorString = "Required compSettings \"Band6\" is not a list of floats"
        if let band6ListFloat = band6List as? [Float] { // this value is going to used for update
                        
        errorString = "Requied compSettings \"Band6\" does not contain \(BandNumParamsInt) gains"
        if band6ListFloat.count == BandNumParamsInt { // final check for low
                            
        errorString = "Required compSettings \"Band7\" not provided"
        if let band7 = compDict["Band7"] {
                                
        errorString = "Required compSettings \"Band7\" is not a list"
        if let band7List = band7 as? [Any] {
                                    
        errorString = "Required compSettings \"Band7\" is not a list of floats"
        if let band7ListFloat = band7List as? [Float] { // this value is going to used for update
                                        
        errorString = "Requied compSettings \"Band7\" does not contain \(BandNumParamsInt) gains"
        if band7ListFloat.count == BandNumParamsInt { // final check for low
                                            
        errorString = "Required compSettings \"Band8\" not provided"
        if let band8 = compDict["Band8"] {
                                                
        errorString = "Required compSettings \"Band8\" is not a list"
        if let band8List = band8 as? [Any] {
                                                    
        errorString = "Required compSettings \"Band8\" is not a list of floats"
        if let band8ListFloat = band8List as? [Float] { // this value is going to used for update
                                                        
        errorString = "Requied compSettings \"Band8\" does not contain \(BandNumParamsInt) gains"
        if band8ListFloat.count == BandNumParamsInt { // final check for low
                                                            
        errorString = "Required compSettings \"Band9\" not provided"
        if let band9 = compDict["Band9"] {
                                                                
        errorString = "Required compSettings \"Band9\" is not a list"
        if let band9List = band9 as? [Any] {
                                                                    
        errorString = "Required compSettings \"Band9\" is not a list of floats"
        if let band9ListFloat = band9List as? [Float] { // this value is going to used for update
                                                                        
        errorString = "Requied compSettings \"Band9\" does not contain \(BandNumParamsInt) gains"
         if band9ListFloat.count == BandNumParamsInt { // final check for low
                                                                                                
        // ***** End compression settings **********
                                                                         
        iosAudio.settings.pointee.compressionOutputType = Int32(switchComp);
        //iosAudio.settings.pointee.noiseReductionOutputType =  Int32(switchNoiseRedInt);
        //iosAudio.settings.pointee.afcSwitch = Int32(switchAFC);
        //UIView is a class, and setNeedsDisplay() is an instance function.
                                                                                                
        //view.setNeedsDisplay();
            
        //this.Invoke((MethodInvoker)delegate {
        //switchOutputType.isOn = audioController.noiseReductionOutputType;
                                                                                                    
        //});
        var band1Param = band1ListFloat;var band2Param = band2ListFloat; var band3Param = band3ListFloat; var band4Param = band4ListFloat;
        var band5Param = band5ListFloat; var band6Param = band6ListFloat; var band7Param = band7ListFloat; var band8Param = band8ListFloat;
        var band9Param = band9ListFloat;

        for i in 0...BandNumParamsInt {
            if i == 0 { // ignore 2nd compression threshold at 75 dB
                band1Param[i] = floor(  ( band1Param[i] + band2Param[i] )/2  );
            } else if i == 1 { // convert compression threshold between [30-80] to [-50, 0]
                band1Param[i] = floor(  ( band1Param[i] + band2Param[i] )/2  );
                band1Param[i] = (Float) (band1Param[i] - Float(80));
            } else if (i == 3 || i == 4) {
                band1Param[i-1] = floor(  ( band1Param[i] + band2Param[i] )/2  );
            }
        }
        for i in 0...BandNumParamsInt {
            if i == 0 { // ignore 2nd compression threshold at 75 dB
                band2Param[i] = floor(  ( band3Param[i] + band4Param[i] )/2  );
            } else if i == 1 { // convert compression threshold between [30-80] to [-50, 0]
                band2Param[i] = floor(  ( band3Param[i] + band4Param[i] )/2  );
                band2Param[i] = (Float) (band2Param[i] - Float(80));
            } else if (i == 3 || i == 4) {
                band2Param[i-1] = floor(  ( band3Param[i] + band4Param[i] )/2  );
            }
        }
        for i in 0...BandNumParamsInt {
            if i == 0 { // ignore 2nd compression threshold at 75 dB
                band3Param[i] = band5Param[i]
            } else if i == 1 { // convert compression threshold between [30-80] to [-50, 0]
                band3Param[i] = band5Param[i]
                band3Param[i] = (Float) (band3Param[i] - Float(80));
            } else if (i == 3 || i == 4) {
                band3Param[i-1] = band5Param[i]
            }
        }
        for i in 0...BandNumParamsInt {
            if i == 0 { // ignore 2nd compression threshold at 75 dB
                band4Param[i] = floor(  ( band6Param[i] + band7Param[i] )/2  );
            } else if i == 1 { // convert compression threshold between [30-80] to [-50, 0]
                band4Param[i] = floor(  ( band6Param[i] + band7Param[i] )/2  );
                band4Param[i] = (Float) (band4Param[i] - Float(80));
            } else if (i == 3 || i == 4) {
                band4Param[i-1] = floor(  ( band6Param[i] + band7Param[i] )/2  );
            }
        }
        for i in 0...BandNumParamsInt {
            if i == 0 { // ignore 2nd compression threshold at 75 dB
                band5Param[i] = floor(  ( band8Param[i] + band9Param[i] )/2  );
            } else if i == 1 { // convert compression threshold between [30-80] to [-50, 0]
                band5Param[i] = floor(  ( band8Param[i] + band9Param[i] )/2  );
                band5Param[i] = (Float) (band5Param[i] - Float(80));
            } else if (i == 3 || i == 4) {
                band5Param[i-1] = floor(  ( band8Param[i] + band9Param[i] )/2  );
            }
        }
        
        // create vectors with only 4 parameters each
            var band1Final = [Float](repeating: 0, count: 4);
            var band2Final = [Float](repeating: 0, count: 4);
            var band3Final = [Float](repeating: 0, count: 4);
            var band4Final = [Float](repeating: 0, count: 4);
            var band5Final = [Float](repeating: 0, count: 4);
        for j in 0...3 {
            band1Final[j] = band1Param[j];
            band2Final[j] = band2Param[j];
            band3Final[j] = band3Param[j];
            band4Final[j] = band4Param[j];
            band5Final[j] = band5Param[j];
        }
        compressionTable["Band1"] = band1Final
        compressionTable["Band2"] = band2Final
        compressionTable["Band3"] = band3Final
        compressionTable["Band4"] = band4Final
        compressionTable["Band5"] = band5Final
        iosAudio.settings.pointee.compressionBand1 = UnsafeMutablePointer<Float>(mutating:compressionTable["Band1"]!)
        iosAudio.settings.pointee.compressionBand2 = UnsafeMutablePointer<Float>(mutating:compressionTable["Band2"]!)
        iosAudio.settings.pointee.compressionBand3 = UnsafeMutablePointer<Float>(mutating:compressionTable["Band3"]!)
        iosAudio.settings.pointee.compressionBand4 = UnsafeMutablePointer<Float>(mutating:compressionTable["Band4"]!)
        iosAudio.settings.pointee.compressionBand5 = UnsafeMutablePointer<Float>(mutating:compressionTable["Band5"]!)
                                                                                                
        iosAudio.settings.pointee.jsonFileSelected = 1; // give permission to reflect the compression settings on GUI and compression computations
                                                                                                
        // Everything valid in JSON file
        //_ = self.update(gains: lowGainsListFloat, currentGains: "Low", setOnly: true)
        //_ = self.update(gains: mediumGainsListFloat, currentGains: "Medium", setOnly: true)
        //_ = self.update(gains: highGainsListFloat, currentGains: "High", setOnly: true)
        success = true
              }}}}}}}}}}}} // for compression settings
              }}}}}}}}}}}}} // for compression settings
            }}}}}}}}}}}}}}}} // for compression settings
              }} //}}}} // for switches
                                                                                    
    //}}}}}}}}}}}}}}}}

        return success
    }
    
}
