package app;

import org.json.*;

import java.util.*;
import java.io.*;
import java.security.*;

import j.io.*;

import adapter.*;
import util.*;

public class Main
{    
    private static final String OPTION_FILE = ".uva-cli";

    /** Maps username to account */
    private final Map<String, Account> accts;

    private Account curAcct;
    private Adapter curAdap;

    public static void main(String[] args)
        throws Exception
    {
        Main main = new Main();
    
        String homePath = System.getProperty("user.home");
        String optFilePath = PathUtil.combine(homePath, OPTION_FILE);
        
        try
        {
            main.load(optFilePath);
        }
        catch (FileNotFoundException ex)
        {
            System.out.println("Info: option file not found: "+optFilePath);
            System.out.println("Info: A new one will be created after you quit");
        }
        catch (Exception ex)
        {
            System.out.println("Error loading option file: "+optFilePath);
            System.out.println("Is it corrupted? Please delete it and restart.");
            ex.printStackTrace();
            return;
        }

        main.loop();

        main.save(optFilePath);
    }

    public Main()
    {
        this.accts = new HashMap<String, Account>();
    }

    public void load(String optFilePath)
        throws JSONException, IOException, GeneralSecurityException
    {
        String contents = FileUtil.readAll(optFilePath);
        JSONObject obj = new JSONObject((contents));
        String curAcct = obj.getString("curAcct");
        JSONArray accounts = obj.getJSONArray("accts");

        int len = accounts.length();
        for (int i = 0; i < len; i++)
        {
            JSONObject acct = accounts.getJSONObject(i);
            Account a = new Account(acct);
            this.accts.put(a.getUser(), a);
        }

        this.curAcct = null;
        this.curAdap = null;
        if (curAcct != null && curAcct.length() > 0)
        {
            Account a = this.accts.get(curAcct);

            this.curAcct = a;

            if (this.curAcct != null)
            {
                this.curAdap = Adapter.create(this.curAcct);
                if (this.curAdap == null)
                    this.curAcct = null;
            } 
        }
    }

    public void save(String optFilePath)
        throws JSONException, IOException, GeneralSecurityException
    {
        JSONObject obj = new JSONObject();

        int idx=0;
        JSONArray accounts = new JSONArray();
        for (Map.Entry<String, Account> e : this.accts.entrySet())
        {
            accounts.put(idx++, e.getValue().toJSON());
        }

        obj.put("accts", accounts);
        obj.put("curAcct", this.curAcct != null ? this.curAcct.getUser() : "");

        PrintWriter bw = null;

        try
        {
            bw = new PrintWriter(
                new OutputStreamWriter(new FileOutputStream(optFilePath),"utf-8"));
            bw.print(obj.toString());
            bw.flush();
        }
        finally
        {
            if(bw != null) bw.close();
        }
    }

    private void dispatch(String action, String[] toks)
        throws Exception
    {
        if (action.equals("add"))
        {
            // syntax: add <type> <username> <password>

            if (toks.length != 4)
            {
                System.out.println("Syntax: add <type> <username> <password>");
                return;
            }

            String type = toks[1];
            String user = toks[2];
            String pass = toks[3];


            Account a = new Account(type, user,pass);

            Adapter adap = Adapter.create(a);
            if (adap == null)
            {
                System.out.println("invalid type");
                return;
            }

            Account prev = this.accts.put(user, a);
            if (prev != null)
                System.out.println("account already exists, and overwritten");
            else
                System.out.println("account added ok.");
        }
        else if (action.equals("use"))
        {
            // syntax: use <username> 

            if (toks.length == 1)
            {
                System.out.println("current account set to none");
                this.curAcct = null;
                this.curAdap = null;
                return;
            }

            if (toks.length != 2)
            {
                System.out.println("syntax: use <username>");
                return;
            }

            Account a = this.accts.get(toks[1]);
            if (a==null)
            {
                System.out.println("account does not exist");
                return;
            }

            this.curAcct = a;
            this.curAdap = Adapter.create(a);
        }
        else if (action.equals("remove"))
        {
            // syntax: remove <username> 


            if (toks.length != 2)
            {
                System.out.println("syntax: remove <username>");
                return;
            }

            if (this.curAcct != null && toks[1].equals( this.curAcct.getUser()))
            {
                System.out.println("account is in use. cannot remove");
                return;
            }


            Account a = this.accts.get(toks[1]);
            if (a==null)
            {
                System.out.println("account does not exist");
                return;
            }

            this.accts.remove(toks[1]);
        }
        else if (action.equals("send"))
        {
            if (this.curAdap == null)
            {
                System.out.println("Please select an account to use first");
                return;
            }

            if (toks.length != 3)
            {
                System.out.println("Syntax: send <prob-id> <fileName>");
                return;
            }

            String probId = toks[1];
            String filePath = toks[2];

            System.out.println("Logging in...");
            if (!login()) return;

            System.out.println("Sending code...");
            try
            {
                this.curAdap.send(probId, new File(filePath));
                System.out.println("Sent OK");
            }
            catch(Exception e)
            {
                System.out.println("Error sending: "+e.getMessage());
            }

            //TODO
        }
        else if (action.equals("show"))
        {
            for(Account acct : this.accts.values())
            {
                System.out.println("type="+acct.getType() + " username="+acct.getUser());
            }
        }
        else if (action.equals("status"))
        {
            // TODO
        }
        else 
        {
            System.out.println("Unrecognized action: "+action);
        }
    }

    private boolean login()
    {
        try
        {
            this.curAdap.login();
            return true;
        }
        catch (Exception ex)
        {
            System.out.println("Cannot login: "+ex.getMessage());       
        }

        return false;
    }

    public void loop() 
        throws Exception
    {
        Scanner s = new Scanner(System.in);

        String line;
        while (true)
        {
            System.out.print("> ");

            line = s.nextLine();
            
            // end of stream?
            if (line == null) break;

            line = line.trim();
            if (line.length() == 0)
                continue;

            String toks[] = line.split("\\s+");
            String action = toks[0].toLowerCase();

            if (action.equals("quit") || action.equals("exit"))
                return;

            dispatch(action, toks);

            System.out.println();
        }
    }
}
