void autozoom(TH2F * h)
{
    TH1F * htmp = new TH1F("htmp", "htmp", 1000, -1.5, 1.5);
    float b = 0;
    for (int ix = 0; ix < h->GetNbinsX(); ++ix)
    {
        for (int iy = 0; iy < h->GetNbinsY(); ++iy)
        {
            b = h->GetBinContent(ix, iy);
            if (b != 0) htmp->Fill(b);
        }
    }
    float tot = htmp->GetEntries();
    float xlow = 0, xhigh = 0;
    float inte = 0;
    for (int ix = 0; ix < htmp->GetNbinsX(); ++ix)
    {
        inte += htmp->GetBinContent(ix);
        if (!xlow  && inte / tot > 0.025) xlow  = htmp->GetBinCenter(ix);
        if (!xhigh && inte / tot > 0.975) xhigh = htmp->GetBinCenter(ix);
    }
    h->SetMaximum(xhigh);
    h->SetMinimum(xlow);
    htmp->Delete();
}
