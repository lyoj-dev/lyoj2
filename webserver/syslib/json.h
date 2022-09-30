string json_encode(Json::Value val) {
    Json::FastWriter writer;
    return writer.write(val);
}

string json_pretty_encode(Json::Value val) {
    Json::StreamWriterBuilder jswBuilder;
    jswBuilder["emitUTF8"] = true;
    jswBuilder["indentation"] = "  ";
    jswBuilder["enableYAMLCompatibility"] = true;
    unique_ptr<Json::StreamWriter> writer(jswBuilder.newStreamWriter());
    ostringstream buffer;
    writer->write(val, &buffer);
    return buffer.str();
}

Json::Value json_decode(string json) {
    Json::Reader reader;
    Json::Value val;
    reader.parse(json, val);
    return val;
}