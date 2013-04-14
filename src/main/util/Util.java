package util;

import javax.net.ssl.*;
import java.net.*;
import java.io.*;
import java.util.*;

import j.str.*;

public class Util
{
    /**
     * A simple HTML entities decoder.
     */
    public static String htmlDecodeSimple(String str)
    {
        return str.replace("&lt;", "<").replace("&gt;", ">")
                .replace("&quot;", "\"").replace("&apos;", "'")
                .replace("&amp;", "&");
    }

    public static String buildQuery(Map<?,?> data)
        throws UnsupportedEncodingException
    {
        String sep = "";
        StringBuilder sb = new StringBuilder();
        for (Map.Entry<?,?> pair : data.entrySet())
        {
            String key = pair.getKey().toString();
            String value = pair.getValue().toString();
            sb.append(sep);
            sb.append(URLEncoder.encode(key, "UTF-8"));
            sb.append("=");
            sb.append(URLEncoder.encode(value, "UTF-8"));
            sep = "&";
        }

        return sb.toString();
    }

    /**
     * Creates an HTTP(S) connection populated with common headers.
     * @param url HTTP or HTTPS URL.
     * @param ajax Whether this is an AJAX request.
     */
    public static HttpURLConnection createConnection(
            URL url, 
            boolean ajax)
        throws IOException
    {
        HttpURLConnection conn = (HttpURLConnection)(url.openConnection());
        
        conn.setRequestProperty("Referer", url.toString());
        conn.setRequestProperty("Accept-Charset", "ISO-8859-1,utf-8");
        // no gzip :(
        //conn.setRequestProperty("Accept-Encoding", "gzip,deflate");
        conn.setRequestProperty("Accept-Language", "en-US,en;q=0.8");
        conn.setRequestProperty("User-Agent", 
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_2) "+
            "AppleWebKit/537.17 (KHTML, like Gecko) "+
            "Chrome/24.0.1312.57 Safari/537.17");
        
        if (ajax)
        {
            conn.setRequestProperty("X-Requested-With", "XMLHttpRequest");
            conn.setRequestProperty("X-Prototype-Version", "1.6.0");
            conn.setRequestProperty("Accept", "text/javascript, text/html, "+
                "application/xml, text/xml, */*");
        }
        else
        {
            conn.setRequestProperty("Accept", "text/html, application/xml, "+
                "text/xml, */*");
        }

        return conn;
    }

    public static void writePostData(HttpURLConnection conn, Map<?,?> data)
        throws IOException
    {
        conn.setRequestMethod("POST");
        String query = buildQuery(data);

        byte[] queryBytes = query.getBytes("UTF-8");
        conn.setRequestProperty("Content-Length", 
            String.valueOf(queryBytes.length)); 
        conn.setRequestProperty("Content-Type",
            "application/x-www-form-urlencoded; charset=UTF-8"); 
        conn.setDoOutput(true); 

        OutputStream os = conn.getOutputStream();
        os.write(queryBytes);
        os.close();
    }

    /**
     * Writes multipart/form-data
     */
    public static void writeFormData(HttpURLConnection conn, Map<?,?> data)
        throws IOException
    {
        Random r = new Random();
        byte b[]= new byte[16];
        r.nextBytes(b);
        String bound = StrUtil.toHex(b);
        byte []boundBytes = ("--"+bound).getBytes("UTF-8");

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        for (Map.Entry<?,?> e : data.entrySet())
        {
            String key = e.getKey().toString();
            out.write(boundBytes);

            Object v = e.getValue();
            if (v instanceof File)
            {
                File file = (File)v;
                byte[] contents = readBinary(file);
                out.write(("\r\nContent-Disposition: form-data; name=\""+ key +"\"; filename=\""+file.getName()+"\"\r\n"+
                            "Content-Type: application/octet-stream\r\n"+
                            "Content-Transfer-Encoding: binary\r\n\r\n").getBytes("utf-8"));
                
                out.write(contents);
            }
            else
            {
                String val = v.toString();
                out.write(("\r\nContent-Disposition: form-data; name=\""+ key +"\"\r\n\r\n").getBytes("utf-8"));
                out.write(val.getBytes("utf-8"));
            }

            out.write("\r\n".getBytes("utf-8"));
            
        }

        out.write(boundBytes);
        out.write("--\r\n".getBytes("utf-8"));

        byte[] all = out.toByteArray();

        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Length", 
            String.valueOf(all.length)); 
        conn.setRequestProperty("Content-Type",
            "multipart/form-data; boundary="+bound); 
        conn.setDoOutput(true); 

        OutputStream os = conn.getOutputStream();
        os.write(all);
        os.close();
    }

    /**
     * Gets a semi-colon-separated list of cookies from the Set-Cookie headers,
     * without the cookies' metadata such as expiry and path.
     * The cookie keys and values are not decoded.
     * @return null if the cookies are not found.
     */
    public static String getCookies(URLConnection conn)
        throws UnsupportedEncodingException
    {
        Map<String,List<String>> fields = conn.getHeaderFields();

        List<String> cookieHeaders = fields.get("Set-Cookie");

        if (cookieHeaders == null) return null;

        StringBuilder sb = new StringBuilder();
        String sep = "";
        for (String cookieHeader : cookieHeaders)
        {
            String[] cookie = Util.getCookie(cookieHeader, false);
            if (cookie == null) continue;
            sb.append(sep + cookie[0] + "=" + cookie[1]);
            sep = "; ";
        }

        return sb.length() > 0 ? sb.toString() : null;
    }

    /** 
     * Retrieves the cookie key and value from a Set-Cookie header 
     * @param line Raw header
     * @param decode whether to decode the key and value
     * @return null if no cookie is found; otherwise an array of two strings,
     *         corresponding to the key and value
     */
    public static String[] getCookie(String line, boolean decode)
        throws UnsupportedEncodingException
    {
        String[] tokens = line.split(";");
        // Cookie should be the first token
        if (tokens.length >= 1)
        {
            String[] pair = tokens[0].split("=");
            if (pair.length != 2) return null;

            String key = pair[0].trim();
            String value = pair[1].trim();
            
            if (decode)
            {
                key = URLDecoder.decode(key, "UTF-8");
                value = URLDecoder.decode(value, "UTF-8");
            }

            return new String[]{key, value};
        }

        return null;
    }

    /**
     * Reads an entire binary file.
     */
    public static byte[] readBinary(File file)
        throws IOException
    {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        byte buf[] = new byte[1<<16];
        
        FileInputStream fis = new FileInputStream(file);
        while (true)
        {
            int nread = fis.read(buf,0,buf.length);
            if (nread < 0) break;
            bos.write(buf, 0, nread);
        }

        fis.close();
        return bos.toByteArray();
    }

    public static String readAll(HttpURLConnection conn)
        throws IOException
    {
        BufferedReader br = null;
        StringBuilder sb = new StringBuilder();

        try
        {
            br = new BufferedReader(new InputStreamReader(
                    conn.getInputStream(), "utf-8"));

            String line;
            while ((line = br.readLine() ) !=null)
            {
                sb.append(line);
                sb.append("\r\n");
            }

            return sb.toString();
        }
        finally
        {
            if (br != null) br.close();
        }
    }
}