package adapter;

import app.*;
import j.web.WebUtil;
import org.json.JSONArray;
import org.json.JSONObject;
import util.*;

import java.io.File;
import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class UVA extends Adapter {
  private static final URL BASE_URL;
  private static final URL SUBMIT_URL;
  private static final URL SUBMIT_PAGE_URL;

  static {
    try {
      SUBMIT_PAGE_URL = new URL("http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=25");

      SUBMIT_URL = new URL("http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=25&page=save_submission");

      BASE_URL = new URL("http://uva.onlinejudge.org");
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
  }

  private static final Pattern FORM_PATTERN =
          Pattern.compile(
                  // group 1: form attribs
                  // group 2: form HTML contents
                  "<form([^>]+?id\\s*=\\s*[\"']?\\w*login[^>]*)>(.*?)" +
                          "</form>",
                  Pattern.CASE_INSENSITIVE |
                          Pattern.DOTALL);
  private static final Pattern INPUT_PATTERN =
          Pattern.compile(
                  // group 1: attribs
                  "<input([^>]+?)/?>",
                  Pattern.CASE_INSENSITIVE |
                          Pattern.DOTALL);
  private String cookies;
  private int userId;

  public UVA(Account acct) {
    super(acct);

    this.userId = -1;
  }

  private static URL combineUrl(String url) throws MalformedURLException {
    if (url.startsWith("http://") || url.startsWith("https://"))
      return new URL(url);

    return new URL(BASE_URL, url);
  }

  private static Form parseForm(Pattern formPat, String html) {
    Matcher m = formPat.matcher(html);

    if (!m.find())
      return null;

    Form f = new Form();

    String attribs = m.group(1);
    String contents = m.group(2);

    Map<String, String> atts = WebUtil.parseAttrib(attribs);
    for (Map.Entry<String, String> e : atts.entrySet()) {
      String key = e.getKey();
      if (key.equalsIgnoreCase("action"))
        f.action = Util.htmlDecodeSimple(e.getValue());
    }

    f.contents = contents;
    f.data = new HashMap<String, String>();

    m = INPUT_PATTERN.matcher(contents);
    while (m.find()) {
      atts = WebUtil.parseAttrib(m.group(1));
      String value = null;
      String name = null;
      boolean isText = false;

      for (Map.Entry<String, String> e : atts.entrySet()) {
        String val = Util.htmlDecodeSimple(e.getValue());
        String key = (e.getKey());
        String keyLower = key.toLowerCase();

        if (keyLower.equals("type")) {
          isText = val.equalsIgnoreCase("password") ||
                  (val.equalsIgnoreCase("text"));

        } else if (keyLower.equals("name")) {
          name = (val);
        } else if (keyLower.equals("value"))
          value = (val);
      }

      if (name != null && isText) {
        String nameLower = name.toLowerCase();
        if (nameLower.indexOf("user") >= 0)
          f.userField = name;
        else if (nameLower.indexOf("pass") >= 0)
          f.passField = name;
      } else if (value != null && name != null) {
        f.data.put(name, value);
      }
    }

    return f;
  }

  private static String getVerdict(int ver) {
    switch (ver) {
      case 10:
        return "Subm Error";
      case 15:
        return "Can't queue";
      case 20:
        return "In queue";
      case 30:
        return "Compile Err";
      case 35:
        return "Restricted func";
      case 40:
        return "Runtime Err";
      case 45:
        return "output limit";
      case 50:
        return "time limit";
      case 60:
        return "mem limit";
      case 70:
        return "wrong ans";
      case 80:
        return "presentation";
      case 90:
        return "accepted";
    }

    return "?";
  }

  private static String getLang(int id) {
    switch (id) {
      case 1:
        return "C";
      case 2:
        return "Java";
      case 3:
        return "C++";
      case 4:
        return "Pascal";
    }

    return "?";
  }

  private int fetchUserId() throws Exception {
    if (userId > 0)
      return this.userId;

    HttpURLConnection conn = Util.createConnection(
            new URL("http://uhunt.felix-halim.net/api/uname2uid/" + this.acct.getUser()), false);

    String all = Util.readAll(conn);

    try {
      return this.userId = Integer.parseInt(all.trim());
    } catch (Exception e) {
      throw new RuntimeException("Cannot parse user ID: " + all, e);
    }
  }

  @Override
  public void login() throws IOException {
    HttpURLConnection conn = Util.createConnection(BASE_URL, false);
    conn.setRequestProperty("Referer", BASE_URL.toString());
    String html = Util.readAll(conn);
    Form f = parseForm(FORM_PATTERN, html);
    if (f == null)
      throw new RuntimeException("cannot find HTML form");
    if (f.userField == null)
      throw new RuntimeException("cannot find user field");
    if (f.passField == null)
      throw new RuntimeException("cannot find pass field");
    if (f.action == null)
      throw new RuntimeException("cannot find action");

    String sessionCookies = Util.getCookies(conn);

    conn = Util.createConnection(
            combineUrl(f.action), false
    );

    conn.setRequestProperty("Referer", BASE_URL.toString());
    conn.setRequestProperty("Cookie", sessionCookies);

    f.data.put(f.userField, this.acct.getUser());
    f.data.put(f.passField, this.acct.getPass());

    // Because UVA will redirect and we will lose the cookies
    conn.setInstanceFollowRedirects(false);
    Util.writePostData(conn, f.data);
    Util.readAll(conn);

    sessionCookies = Util.getCookies(conn);
    this.cookies = sessionCookies;
  }

  @Override
  protected void send(String probId, Lang lang, File contents) throws IOException {
    HttpURLConnection conn = Util.createConnection(
            SUBMIT_PAGE_URL, false
    );

    conn.setRequestProperty("Cookie", this.cookies);
    Util.readAll(conn);

    conn = Util.createConnection(
            SUBMIT_URL, false
    );

    conn.setRequestProperty("Referer", SUBMIT_PAGE_URL.toString());
    conn.setRequestProperty("Cookie", this.cookies);

    String langVal = null;

    if (lang.equals(Lang.C)) langVal = "1";
    else if (lang.equals(Lang.JAVA)) langVal = "2";
    else if (lang.equals(Lang.CPP)) langVal = "3";
    else if (lang.equals(Lang.PASCAL)) langVal = "4";
    else throw new RuntimeException("unacceptable programming lang");

    Map<String, Object> data = new HashMap<String, Object>();
    data.put("localid", probId);
    data.put("code", "");
    data.put("language", langVal);
    data.put("codeupl", contents);
    data.put("problemid", "");
    data.put("category", "");

    // Because UVA will redirect
    conn.setInstanceFollowRedirects(false);

    Util.writeFormData(conn, data);
    Util.readAll(conn);

  }

  @Override
  public void getUserRank() throws Exception {
    HttpURLConnection conn = Util.createConnection(
            new URL("http://uhunt.felix-halim.net/api/ranklist/" + this.fetchUserId() + "/0/0"), false
    );

    String all = Util.readAll(conn);
    JSONObject obj = new JSONArray(all).getJSONObject(0);
    JSONArray activity = obj.getJSONArray("activity");
    String[] text = new String[]{
            "Name: " + obj.getString("name"),
            "Rank: " + obj.getString("rank"),
            "AC: " + obj.getString("ac"),
            "Submissions: "+obj.getString("nos"),
            "Activity: "+activity.toString()
    };
    for (String s : text)
      System.out.println(s);
  }

  @Override
  public void printStatus() throws Exception {
    final int userId = this.fetchUserId();

    HttpURLConnection conn = Util.createConnection(
            new URL("http://uhunt.felix-halim.net/api/subs/" + userId), false
    );

    String all = Util.readAll(conn);
    JSONObject obj = new JSONObject(all);
    Object subStr = obj.get("subs");
    JSONArray subs = null;
    if (subStr instanceof String) {
      String str = (String) subStr;
      subs = new JSONArray(str);
    } else {
      subs = (JSONArray) subStr;
    }

    int len = subs.length();

    List<JSONArray> list = new ArrayList<JSONArray>();
    for (int i = 0; i < len; i++) {
      list.add(subs.getJSONArray(i));
    }

    // sort by subm id. Latest will be at 0th elem.
    Collections.sort(list, new Comparator<JSONArray>() {
      @Override
      public int compare(JSONArray a1, JSONArray a2) {
        try {
          long k = a2.getLong(0) - a1.getLong(0);
          if (k < 0) return -1;
          if (k > 0) return 1;
          return 0;
        } catch (Exception e) {
          // omg so fcking annoying with exceptions.
          throw new RuntimeException(e);
        }
      }
    });

    System.out.println("SubId     | ProbId |      Verdict     |  Lang  | Runtime | ");
    //                  123456789---123456---1234567890123456---123456---1234567---

    int iterLen = Math.min(10, list.size());
    for (int i = 0; i < iterLen; i++) {
      JSONArray sub = list.get(i);

            /*
            Format:
            Submission ID
            Problem ID
            Verdict ID
            Runtime
            Submission Time (unix timestamp)
            Language ID (1=ANSI C, 2=Java, 3=C++, 4=Pascal)
            Submission Rank
            */

      long subId = sub.getInt(0);
      int probId = sub.getInt(1);
      int verdictId = sub.getInt(2);
      int runtime = sub.getInt(3);
      long time = sub.getLong(4);
      int langId = sub.getInt(5);
      int rank = sub.getInt(6);

      System.out.format("% 9d   % 6d   %16s   %6s   % 3d.%03d\n",
              subId, probId, getVerdict(verdictId),
              getLang(langId), runtime / 1000, runtime % 1000);
    }
  }

  private static class Form {
    public String contents;
    public String action;
    public String userField;
    public String passField;
    public Map<String, String> data;
  }
}
