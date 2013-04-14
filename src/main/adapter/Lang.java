package adapter;


public enum Lang
{
    JAVA("java"), 
    C("c"), 
    CPP("cpp"), 
    PASCAL("p", "pas", "pascal");   

    private String[] _exts;

    Lang(String ... exts)
    {
        this._exts = exts;
    }

    public String[] exts()
    {
        return this._exts;
    }
}
