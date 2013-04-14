package adapter;

import j.io.*;
import j.web.*;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.*;
import javax.net.ssl.*;

import util.*;
import app.*;

public class UVA extends Adapter
{
    private static final URL BASE_URL;
    private static final URL SUBMIT_URL;
    private static final URL SUBMIT_PAGE_URL;

    static
    {
        try
        {
            SUBMIT_PAGE_URL = new URL("http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=25");

            SUBMIT_URL = new URL("http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=25&page=save_submission");

            BASE_URL = new URL("http://uva.onlinejudge.org");
        }
        catch(Exception e){throw new RuntimeException(e);}
    }

    private static final Pattern FORM_PATTERN = 
        Pattern.compile(
        // group 1: form attribs
        // group 2: form HTML contents
            "<form([^>]+?id\\s*=\\s*[\"']?\\w*login[^>]*)>(.*?)"+
            "</form>", 
                Pattern.CASE_INSENSITIVE | 
                Pattern.DOTALL);

    private static final Pattern INPUT_PATTERN = 
        Pattern.compile(
        // group 1: attribs
            "<input([^>]+?)/?>",
                Pattern.CASE_INSENSITIVE | 
                Pattern.DOTALL);

    private static class Form
    {
        public String contents;
        public String action;
        public String userField;
        public String passField;
        public Map<String,String> data;
    }

    private static URL combineUrl(String url)
        throws MalformedURLException
    {
        if (url.startsWith("http://") || url.startsWith("https://"))
            return new URL( url);

        return new URL(BASE_URL, url);
    }

    private static Form parseForm(Pattern formPat, String html)
    {
        Matcher m = formPat.matcher(html);

        if (!m.find())
            return null;

    
        Form f = new Form();

        String attribs = m.group(1);
        String contents = m.group(2);

        Map<String,String> atts = WebUtil.parseAttrib(attribs);
        for (Map.Entry<String,String> e : atts.entrySet())
        {
            String key = e.getKey();
            if (key.equalsIgnoreCase("action"))
                f.action = Util.htmlDecodeSimple(e.getValue());
        }

        f.contents = contents;
        f.data = new HashMap<String,String>();

        m = INPUT_PATTERN.matcher(contents);
        while(m.find())
        {
            atts = WebUtil.parseAttrib(m.group(1));
            String value=null;
            String name=null;
            boolean isText = false;

            for (Map.Entry<String,String> e : atts.entrySet())
            {
                String val = Util.htmlDecodeSimple(e.getValue());
                String key = (e.getKey());
                String keyLower = key.toLowerCase();

                if (keyLower.equals("type"))
                {
                    isText = val.equalsIgnoreCase("password")|| 
                        (val.equalsIgnoreCase("text"));
                    
                }
                else if (keyLower.equals("name"))
                {
                    name = (val);
                }
                else if (keyLower.equals("value"))
                    value = (val);

            }
    
            if (name != null && isText)
            {
                String nameLower = name.toLowerCase();
                if (nameLower.indexOf("user") >= 0)
                    f.userField=name;
                else if (nameLower.indexOf("pass")>=0)
                    f.passField = name;
            }
            else if (value != null && name != null)
            {
                f.data.put(name,value);
            }
        }

        return f;
    }

    private String cookies;

    public UVA(Account acct)
    {
        super(acct);
    }

    @Override
    public void login()
        throws IOException
    {
        
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
    protected void send(String probId, Lang lang, File  contents)
        throws IOException
    {
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

        Map<String,Object> data = new HashMap<String,Object>();        
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
}
