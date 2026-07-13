function doPost(e) {
  if(!e || !e.postData || !e.postData.contents){
    return ContentService
      .createTextOutput(JSON.stringify({status: "error", reason: "no data"}))
      .setMimeType(ContentService.MimeType.JSON);
  }

  const sheet = SpreadsheetApp
    .getActive()
    .getSheetByName("data");

  const payload = JSON.parse(e.postData.contents);

  if(!Array.isArray(payload.rows)){
    throw new Error("Invalid payload");
  }

  const rows = payload.rows.map(row => ([
    new Date(),           // timestamp
    row[0],       // node
    row[1],       // sensor
    row[2],       // metric
    row[3],       // value
    row[4],       // unit
    row[5] ? String(row[5]): ""  // quality
  ]));

  sheet.getRange(
    sheet.getLastRow() + 1,
    1,
    rows.length,
    rows[0].length
  ).setValues(rows);

  

  return ContentService
    .createTextOutput(JSON.stringify({ status: "ok", inserted: rows.length }))
    .setMimeType(ContentService.MimeType.JSON);
}






function doGet(e) {
  const sheet = SpreadsheetApp
    .getActive()
    .getSheetByName("data");

  const data = sheet.getDataRange().getValues();
  const headers = data.shift();

  const filters = e.parameter;
  const limit = Number(filters.limit || 50);

  const result = [];

  for (const row of data.reverse()) {
    const entry = Object.fromEntries(
      headers.map((h, i) => [h, row[i]])
    );

    if (filters.node && entry.node !== filters.node) continue;
    if (filters.sensor && entry.sensor !== filters.sensor) continue;
    if (filters.metric && entry.metric !== filters.metric) continue;

    result.push(entry);
    if (result.length >= limit) break;
  }

  return ContentService
    .createTextOutput(JSON.stringify(result))
    .setMimeType(ContentService.MimeType.JSON);
}

