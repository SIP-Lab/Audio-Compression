package com.superpowered.IntegratedApp;
import android.content.Intent;
import android.media.Image;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.util.ArrayList;

import android.text.Html;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.ToggleButton;
import java.io.*;



public class settingsFromJsonFile extends AppCompatActivity implements View.OnClickListener {

    // https://developer.android.com/guide/topics/ui/declaring-layout#AdapterViews
        int[] id_buttons = {R.id.json1, R.id.json2, R.id.json3, R.id.json4, R.id.json5, R.id.json6, R.id.json7};
        int[] id_jsonTick = {R.id.json1Tick, R.id.json2Tick, R.id.json3Tick, R.id.json4Tick, R.id.json5Tick, R.id.json6Tick, R.id.json7Tick};
        private static int maxJsonNum = 10;
        Button[] ButtonArray = new Button[maxJsonNum];
        ImageView[] jsonTick = new ImageView[maxJsonNum];
        private int numExistingJsonFiles = 0;
        private String[] list = {};

        private static final String FILE_TEXT_EXT = ".json";
        private static final int READ_REQUEST_CODE = 42;
        String JSON_Folder_Dir = Environment.getExternalStorageDirectory().toString() + "/IntegratedApp_Android/";
        ArrayList<String> jsonFileArray = new ArrayList<>();

        private int compressionSwitch ;
        private int noiseReductionSwitch ;
        private int AFCswitch ;
        private int CompNumParams ;
        private float[] FirstBand  = new float[4];
        private float[] secondBand = new float[4];
        private float[] thirdBand  = new float[4];
        private float[] FourthBand  = new float[4];
        private float[] FifthBand  = new float[4];


        @Override
        protected void onCreate (Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings_from_json_file);

        //load the Settings structure which pass user data from between jni and java
        System.loadLibrary("FrequencyDomain");

        // create the folder if it does not exist
        File appFolder = new File(JSON_Folder_Dir);
        if (!appFolder.exists()) {
            appFolder.mkdirs();
        }

        // to find files with certain extension only (with .json externsion only)
        listFile(JSON_Folder_Dir, FILE_TEXT_EXT);

        for (int i = 0; i < numExistingJsonFiles; i++) {
            ButtonArray[i] = (Button) findViewById(id_buttons[i]);
            ButtonArray[i].setText(String.format(list[i].toString()));
            ButtonArray[i].setOnClickListener(this);
        }
    }

        @Override
        public void onClick (View v){
        // TODO Auto-generated method stub
        int i;
        Intent intentMainActivity = null; //An intent is an abstract description of an operation to be performed. It can be used with startActivity to launch an Activity

        switch (v.getId()) { // this can is only capable of reading 5 json files, ...
            case R.id.json1:
                i = 0;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, Html.fromHtml("<font color='#FF3352' ><b>" + list[i].toString() + " is selected " + "</b></font>"), 1000).show();
                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;
            case R.id.json2:
                i = 1;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, Html.fromHtml("<font color='#3933FF' ><b>" + list[i].toString() + " is selected " + "</b></font>"), 1000).show();

                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;
            case R.id.json3:
                i = 2;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, list[i].toString() + " is selected ", 1000).show();

                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;

            case R.id.json4:
                i = 3;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, list[i].toString() + " is selected ", 1000).show();

                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;

            case R.id.json5:
                i = 4;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, list[i].toString() + " is selected ", 1000).show();

                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;

            case R.id.json6:
                i = 5;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, list[i].toString() + " is selected ", 1000).show();

                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;

            case R.id.json7:
                i = 6;
                if (i < list.length ) {
                    Toast.makeText(settingsFromJsonFile.this, list[i].toString() + " is selected ", 1000).show();

                    for (int j = 0; j< list.length; j++){
                        jsonTick[j] = (ImageView) findViewById(id_jsonTick[j]);
                        if (j == i){jsonTick[j].setVisibility(View.VISIBLE);
                        }   else   {jsonTick[j].setVisibility(View.GONE);}
                    }

                    readJSONFromStorage(i);
                    System.loadLibrary("FrequencyDomain");
                    /*UpdateSettingsFromJSON(compressionSwitch,
                            noiseReductionSwitch,
                            AFCswitch,
                            CompNumParams);*/

                    intentMainActivity = new Intent(this,MainActivity.class);
                    if (compressionSwitch == 1){ intentMainActivity.putExtra("compressionSwitchUpdate", true);}else {intentMainActivity.putExtra("compressionSwitchUpdate", false);}
                    if (noiseReductionSwitch == 1){ intentMainActivity.putExtra("noiseReductionSwitchUpdate", true);}else {intentMainActivity.putExtra("noiseReductionSwitchUpdate", false);}
                    intentMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null!=intentMainActivity) startActivity(intentMainActivity);
                    finish();

                    Intent intentCompression = null;
                    intentCompression = new Intent(this, CompressionSettings.class);
                    intentCompression.putExtra("Band1Update", FirstBand);
                    intentCompression.putExtra("Band2Update", secondBand);
                    intentCompression.putExtra("Band3Update", thirdBand);
                    intentCompression.putExtra("Band4Update", FourthBand);
                    intentCompression.putExtra("Band5Update", FifthBand);
                    //intentCompression.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    if (null != intentCompression) startActivity(intentCompression);
                    finish();

                }
                break;
        }

    } // end of onClick

    public static String pack(float[] data) {
        StringBuilder sb = new StringBuilder();
        final int length = data.length;
        for (int i = 0; i < length; i++) {
            sb.append(data[i]);
            if (i < (length - 1)) {
                sb.append(':');
            }
        }
        return sb.toString();
    }

    public static float[] unpack(String str) {
        if (TextUtils.isEmpty(str)) {
            return new float[0]; // or null depending on your needs
        } else {
            String[] srtData = TextUtils.split(str, ":");
            final int length = srtData.length;
            float[] result = new float[length];
            for (int i = 0; i < length; i++) {
                result[i] = Float.parseFloat(srtData[i]);
            }
            return result;
        }
    }

        //******** to find files with certain extension only (with .json externsion only)***********
        public void listFile (String folder, String ext){

        GenericExtFilter filter = new GenericExtFilter(ext);
        File dir = new File(folder);

        if (dir.isDirectory() == false) {
            System.out.println("Directory does not exists : " + JSON_Folder_Dir);
            return;
        }

        // list out all the file name and filter by the extension
        list = dir.list(filter);
        numExistingJsonFiles = list.length;
    }

        // inner class, generic extension filter
        public class GenericExtFilter implements FilenameFilter {

            private String ext;

            public GenericExtFilter(String ext) {
                this.ext = ext;
            }

            public boolean accept(File dir, String name) {
                return (name.endsWith(ext));
            }
        }
        // ***** End finding json files ************************************************************


        public String readJSONFromStorage(int JsonFileNumber){
            String jsonStr = null;
        try {
            File jsonFile = new File(JSON_Folder_Dir, list[JsonFileNumber].toString());
            FileInputStream stream = new FileInputStream(jsonFile);
            try {
                FileChannel fc = stream.getChannel();
                MappedByteBuffer bb = fc.map(FileChannel.MapMode.READ_ONLY, 0, fc.size());

                jsonStr = Charset.defaultCharset().decode(bb).toString();
            }
            catch(Exception e){
                e.printStackTrace();
            }
            finally {
                stream.close();
            }
            if (jsonStr != null) try {

                //JSONObject obj = new JSONObject(readJSONFromStorage(i));
                JSONObject obj = new JSONObject(jsonStr);
                compressionSwitch = obj.getInt("compressionSwitch");
                //noiseReductionSwitch = obj.getInt("noiseReductionSwitch");
                //AFCswitch = obj.getInt("AFCswitch");
                CompNumParams = obj.getInt("CompNumParams") - 1; // we should have only one CR, one CT, and attach , release time (4 parameters)
                JSONArray Band1 = obj.getJSONObject("compressionSettings").getJSONArray("Band1");
                JSONArray Band2 = obj.getJSONObject("compressionSettings").getJSONArray("Band2");
                JSONArray Band3 = obj.getJSONObject("compressionSettings").getJSONArray("Band3");
                JSONArray Band4 = obj.getJSONObject("compressionSettings").getJSONArray("Band4");
                JSONArray Band5 = obj.getJSONObject("compressionSettings").getJSONArray("Band5");
                JSONArray Band6 = obj.getJSONObject("compressionSettings").getJSONArray("Band6");
                JSONArray Band7 = obj.getJSONObject("compressionSettings").getJSONArray("Band7");
                JSONArray Band8 = obj.getJSONObject("compressionSettings").getJSONArray("Band8");
                JSONArray Band9 = obj.getJSONObject("compressionSettings").getJSONArray("Band9");

                //Log.d("compressionSwitch: ", String.valueOf(compressionSwitch));
                //Log.d("noiseReductionSwitch: ", String.valueOf(noiseReductionSwitch));

                // Extract numbers from JSON array. performing averaging on 9 bands to get overall only 5 bands
                for (int i = 0; i < Band1.length(); i++) {
                    if (i == 0) { // ignore 2nd compression threshold at 75 dB
                        FirstBand[i] = (float) Math.ceil((Float.parseFloat(Band1.getString(i)) + Float.parseFloat(Band2.getString(i))) / 2);
                    } else if (i == 1) { // convert compression threshold between [45-95] to [-50, 0]
                        FirstBand[i] = (float) Math.ceil((Float.parseFloat(Band1.getString(i)) + Float.parseFloat(Band2.getString(i))) / 2);
                        FirstBand[i] = (float) (FirstBand[i] - 95);
                    } else if (i == 3 | i == 4){
                        FirstBand[i-1] = (float) Math.ceil((Float.parseFloat(Band1.getString(i)) + Float.parseFloat(Band2.getString(i))) / 2);
                    }

                }

                for (int i = 0; i < Band2.length(); i++) {
                    if (i == 0) { // ignore 2nd compression threshold at 75 dB
                        secondBand[i] = (float) Math.ceil((Float.parseFloat(Band3.getString(i)) + Float.parseFloat(Band4.getString(i))) / 2);
                    } else if (i == 1) { // convert compression threshold between [45-95] to [-50, 0]
                        secondBand[i] = (float) Math.ceil((Float.parseFloat(Band3.getString(i)) + Float.parseFloat(Band4.getString(i))) / 2);
                        secondBand[i] = (float) (secondBand[i] - 95);
                    } else if (i == 3 | i == 4){
                        secondBand[i-1] = (float) Math.ceil((Float.parseFloat(Band3.getString(i)) + Float.parseFloat(Band4.getString(i))) / 2);
                    }
                }

                for (int i = 0; i < Band3.length(); i++) {
                    if (i == 0) { // ignore 2nd compression threshold at 75 dB
                        thirdBand[i] = Float.parseFloat(Band5.getString(i)) ;
                    } else if (i == 1) { // convert compression threshold between [45-95] to [-50, 0]
                        thirdBand[i] = Float.parseFloat(Band5.getString(i));
                        thirdBand[i] = (float) (thirdBand[i] - 95);
                    } else if (i == 3 | i == 4){
                        thirdBand[i-1] = Float.parseFloat(Band5.getString(i)) ;
                    }
                }

                for (int i = 0; i < Band4.length(); i++) {
                    if (i == 0) { // ignore 2nd compression threshold at 75 dB
                        FourthBand[i] = (float) Math.ceil((Float.parseFloat(Band6.getString(i)) + Float.parseFloat(Band7.getString(i))) / 2);
                    } else if (i == 1) { // convert compression threshold between [45-95] to [-50, 0]
                        FourthBand[i] = (float) Math.ceil((Float.parseFloat(Band6.getString(i)) + Float.parseFloat(Band7.getString(i))) / 2);
                        FourthBand[i] = (float) (FourthBand[i] - 95);
                    } else if (i == 3 | i == 4){
                        FourthBand[i-1] = (float) Math.ceil((Float.parseFloat(Band6.getString(i)) + Float.parseFloat(Band7.getString(i))) / 2);
                    }
                }

                for (int i = 0; i < Band5.length(); i++) {
                    if (i == 0) { // ignore 2nd compression threshold at 75 dB
                        FifthBand[i] = (float) Math.ceil((Float.parseFloat(Band8.getString(i)) + Float.parseFloat(Band9.getString(i))) / 2);
                    } else if (i == 1) { // convert compression threshold between [45-95] to [-50, 0]
                        FifthBand[i] = (float) Math.ceil((Float.parseFloat(Band8.getString(i)) + Float.parseFloat(Band9.getString(i))) / 2);
                        FifthBand[i] = (float) (FifthBand[i] - 95);
                    } else if (i == 3 | i == 4){
                        FifthBand[i-1] = (float) Math.ceil((Float.parseFloat(Band8.getString(i)) + Float.parseFloat(Band9.getString(i))) / 2);
                    }
                }

            } catch (JSONException e) {
                e.printStackTrace();
            }
        } catch (IOException ex) {
            ex.printStackTrace();
            return null;
        }
        return jsonStr;
    }


    //********************* List of Function for passing variables to FrequencyDomain.cpp *************************************
    private native void UpdateSettingsFromJSON(int compressionSwitch,
                           int noiseReductionSwitch,
                           int AFCswitch,
                           int CompNumParams);

}// end of implements settingFromJsonFile activity



