# An Educational Tool For Hearing Aid Compression Fitting Via A Web-based Adjusted Smartphone App


This GitHub repository is the code accompaniment of the following paper:
> **An educational tool for hearing aid compression fitting via a web-based adjusted smartphone app**<br>
> Nasim Alamdari, Edward Lobarinas, and Nasser Kehtarnavaz, The University of Texas at Dallas<br>
> to appear in Proceeding of IEEE Conference on Acoustics Speech, and Signal Processing, UK, May 2019<br>
>
> **Abstract:** This paper presents an educational tool to learn about how hearing aid compression fitting is prescribed from a signal processing perspective. An interactive web-based program has been developed based on the widely used DSL-v5 fitting rationale. This program can be accessed and used from any internet browser to generate the parameters of compression curves that correspond to the nine frequency bands used in DSL-v5. These parameters are then transferred to a smartphone in the form of a datafile to be used by a compression app, which operates as a virtual hearing aid, running in real-time on both iOS and Android smartphones. This educational tool was found to be very easy-to-use by signal processing engineers as no programming knowledge is needed for adjusting gain across frequency bands and subsequently running the corresponding compression curves in the smartphone app to appropriately compress input sound signals.

## Resources

All material related to our paper is available via the following links:

|**Link**|Description
|:-------|:----------
|http://www.utdallas.edu/%7Ekehtar/WebBasedFitting.html| Web-based Compression Fitting 
|http://www.utdallas.edu/%7Ekehtar/EducationalCompressionFitting.mp4| A Video Clip Demonstrating Various Components of the Web-page and Compression Smartphone App

## Getting Started

A [User's Guide](UsersGuide-CompressionFitting.pdf) is provided with this project which provides all the description required for running the Compression Fitting on Smartphones.

## Requirement
To run the Android version of the Compression app, it is necessary to have Superpowered SDK which can be obtained from the following link: https://superpowered.com.
Then, adding the path of superpowered in gradle/local.properties:

    superpowered.dir = /.../SuperpoweredSDK/Superpowered


## License and Citation
The codes are licensed under MIT license.

For any utilization of the code content of this repository, the following paper needs to be cited by the user:

- Nasim Alamdari, Edward Lobarinas, and Nasser Kehtarnavaz, "An Educational Tool For Hearing Aid Compression Fitting Via A Web-based Adjusted Smartphone App" to appear in Proceeding of IEEE Conference on Acoustics Speech, and Signal Processing, UK, May 2019.
