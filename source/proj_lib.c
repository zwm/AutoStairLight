//---------------------------------------------------------------------------
// Global Variable Define
//---------------------------------------------------------------------------
STRUCT_INT_CNT data st_x0;
STRUCT_INT_CNT data st_x1;
STRUCT_INT_CNT data st_t0;
STRUCT_INT_CNT data st_t1;

//---------------------------------------------------------------------------
// Global Variable Define
//---------------------------------------------------------------------------
unsigned char inc_check (STRUCT_INT_CNT *p)
{
    unsigned char cur;
    cur = p->cnt;
    if (cur > p->bak)       // normal increase
    {
        p->bak = cur;
        cur = cur - p->bak;
    }
    else if (cur < p->bak)  // overflow increase
    {
        p->bak = cur;
        cur = 256 - p->bak + cur;
    }
    else                    // no increase
    {
        cur = 0;
    }
    return cur;
}
