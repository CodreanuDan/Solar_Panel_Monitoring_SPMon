function doPost(e) {
  const sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Sheet1");

  const data = JSON.parse(e.postData.contents);

  sheet.appendRow([
    data.Date,
    data.Time,
    data.DHT_Temp,
    data.DHT_Hum,
    data.DS_Temp,
    data.BMP_Temp,
    data.Pressure,
    data.LUX,
    data.Spec_Violet,
    data.Spec_White,
    data.Spec_White_Green,
    data.Spec_Green,
    data.Spec_Yellow_Green,
    data.Spec_Yellow,
    data.Spec_Orange_Red,
    data.Spec_Red,
    data.Spec_NIR,
    data.Spec_Clear,
    data.ICH1,
    data.UCH1,
    data.ICH2,
    data.UCH2,
    data.ICH3,
    data.UCH3
  ]);

  return ContentService.createTextOutput("OK");
}

