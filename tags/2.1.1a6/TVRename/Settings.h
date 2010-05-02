#pragma once

#include "CustomName.h"
#include "Searchers.h"
#include "TheTVDB.h"

namespace TVRename {
    using namespace System::Collections;
    using namespace System::Xml;

    ref class ProcessedEpisode;

    public ref class Replacement
    {
    public:
        String ^This;
        String ^That;

        Replacement(String ^a, String ^b)
        {
            This = a;
            That = b;
        }
    };
    typedef System::Collections::Generic::List<Replacement ^> ReplaceList;


    enum { reSeasonEp, reEpSeason, reUsingPathSeasonEp };

    public ref class FilenameProcessorRE
    {
    public:
		bool Enabled;
        String ^RE;
        bool UseFullPath;
        String ^Notes;

        FilenameProcessorRE(bool enabled, String ^re, bool useFullPath, String ^notes)
        {
			Enabled = enabled;
            RE = re;
            UseFullPath = useFullPath;
            Notes = notes;
        }
    };

    typedef Generic::List<FilenameProcessorRE ^> FNPRegexList;

    public ref class TVSettings
    {
    private:
        String ^VideoExtensionsString, ^OtherExtensionsString;
        cli::array<String ^> ^VideoExtensionsArray, ^OtherExtensionsArray;

    public:
		int ParallelDownloads;
        FNPRegexList ^FNPRegexs;
        bool BGDownload;
        Searchers ^TheSearchers;
        bool OfflineMode; // TODO: Make property of thetvdb?
        ReplaceList ^Replacements;
        bool ExportWTWRSS;
        String ^ExportWTWRSSTo;
        int WTWRecentDays;
        int StartupTab;
        //NStyle::Style DefaultNamingStyle;
        CustomName ^NamingStyle;
        bool NotificationAreaIcon;
        int ExportRSSMaxDays;
        int ExportRSSMaxShows;
        bool KeepTogether;
        bool LeadingZeroOnSeason;
        bool ShowInTaskbar;
        bool RenameTxtToSub;
        bool ShowEpisodePictures;
        String ^SpecialsFolderName;

		bool ExportMissingXML;
		String ^ExportMissingXMLTo;
		bool ExportMissingCSV;
		String ^ExportMissingCSVTo;
		bool ExportRenamingXML;
		String ^ExportRenamingXMLTo;
		bool ExportFOXML;
		String ^ExportFOXMLTo;

		bool ForceLowercaseFilenames;
		bool IgnoreSamples;
		int SampleFileMaxSizeMB; // sample file must be smaller than this to be ignored

        String ^GetVideoExtensionsString()
        {
            return VideoExtensionsString;
        }
        String ^GetOtherExtensionsString()
        {
            return OtherExtensionsString;
        }

		static bool OKExtensionsString(String ^s)
        {
            cli::array<String ^> ^t = s->Split(';');
            for each (String ^s in t)
                if ((s == "") || (!s->StartsWith(".")))
                    return false;
            return true;
        }
        void SetVideoExtensionsString(String ^s)
        {
            if (OKExtensionsString(s))
            {
                s = s->ToLower();
                VideoExtensionsString = s;
                VideoExtensionsArray = s->Split(';');
            }
        }
        void SetOtherExtensionsString(String ^s)
        {
            if (OKExtensionsString(s))
            {
                s = s->ToLower();
                OtherExtensionsString = s;
                OtherExtensionsArray = s->Split(';');
            }
        }
        TVSettings()
        {
            SetToDefaults();
        }
        void SetToDefaults()
        {
            FNPRegexs = gcnew FNPRegexList();
            NamingStyle = gcnew CustomName();
            Replacements = gcnew ReplaceList();
            BGDownload = false;
            TheSearchers = gcnew Searchers();
            OfflineMode = false;
            NotificationAreaIcon = false;
            Replacements->Clear();
            Replacements->Add(gcnew Replacement("*","#"));
            Replacements->Add(gcnew Replacement("?",""));
            Replacements->Add(gcnew Replacement(">",""));
            Replacements->Add(gcnew Replacement("<",""));
            Replacements->Add(gcnew Replacement(":","-"));
			Replacements->Add(gcnew Replacement(" "," "));
            Replacements->Add(gcnew Replacement("/","-"));
            Replacements->Add(gcnew Replacement("\\","-"));
            Replacements->Add(gcnew Replacement("|","-"));
            Replacements->Add(gcnew Replacement("\"","'"));
            ExportWTWRSS = false;
            ExportWTWRSSTo = "";
            WTWRecentDays = 7;

			ExportMissingXML = false;
			ExportMissingXMLTo = "";
			ExportMissingCSV = false;
			ExportMissingCSVTo = "";
			ExportRenamingXML = false;
			ExportRenamingXMLTo = "";
			ExportFOXML = false;
			ExportFOXMLTo = "";

			ForceLowercaseFilenames = false;
			IgnoreSamples = true;
			SampleFileMaxSizeMB = 50;

            StartupTab = 0;
            ExportRSSMaxDays = 7;
            ExportRSSMaxShows = 10;
            //DefaultNamingStyle = NStyle::Style::Name_SxxEyy_EpName;
            SetVideoExtensionsString(".avi;.mpg;.mpeg;.mkv;.mp4;.wmv;.divx;.ogm;.qt;.rm");
			SetOtherExtensionsString(".srt;.nfo;.txt");
            KeepTogether = true;
            LeadingZeroOnSeason = false;
            ShowInTaskbar = true;
            RenameTxtToSub = false;
            ShowEpisodePictures = false;
            SpecialsFolderName = "Specials";

			ParallelDownloads = 4;

            FNPRegexs = DefaultFNPList();
        }

        static FNPRegexList ^DefaultFNPList()
        {
            FNPRegexList ^l = gcnew FNPRegexList();

            l->Add(gcnew FilenameProcessorRE(true, "(^|[^a-z])s?(?<s>[0-9]+)[ex](?<e>[0-9]{2,})[^a-z]", false, "3x23 s3x23 3e23 s3e23"));
            l->Add(gcnew FilenameProcessorRE(false, "(^|[^a-z])s?(?<s>[0-9]+)(?<e>[0-9]{2,})[^a-z]", false, "323 or s323 for season 3, episode 23. 2004 for season 20, episode 4."));
			l->Add(gcnew FilenameProcessorRE(false, "(^|[^a-z])s(?<s>[0-9]+)--e(?<e>[0-9]{2,})[^a-z]",false,"S02--E03"));
			l->Add(gcnew FilenameProcessorRE(false, "(^|[^a-z])s(?<s>[0-9]+) e(?<e>[0-9]{2,})[^a-z]",false,"'S02.E04' and 'S02 E04'"));
			l->Add(gcnew FilenameProcessorRE(false, "^(?<s>[0-9]+) (?<e>[0-9]{2,})",false,"filenames starting with '1.23' for season 1, episode 23"));


			l->Add(gcnew FilenameProcessorRE(true, "(^|[^a-z])(?<s>[0-9])(?<e>[0-9]{2,})[^a-z]", false, "Show - 323 - Foo"));
            l->Add(gcnew FilenameProcessorRE(true, "(^|[^a-z])se(?<s>[0-9]+)([ex]|ep|xep)?(?<e>[0-9]+)[^a-z]", false, "se3e23 se323 se1ep1 se01xep01..."));
            l->Add(gcnew FilenameProcessorRE(true, "(^|[^a-z])(?<s>[0-9]+)-(?<e>[0-9]{2,})[^a-z]", false, "3-23 EpName"));
            l->Add(gcnew FilenameProcessorRE(true, "(^|[^a-z])s(?<s>[0-9]+) +- +e(?<e>[0-9]{2,})[^a-z]", false, "ShowName - S01 - E01"));

            l->Add(gcnew FilenameProcessorRE(true, "\\b(?<e>[0-9]{2,}) ?- ?.* ?- ?(?<s>[0-9]+)", false, "like '13 - Showname - 2 - Episode Title.avi'"));
            l->Add(gcnew FilenameProcessorRE(true, "\\b(episode|ep|e) ?(?<e>[0-9]{2,}) ?- ?(series|season) ?(?<s>[0-9]+)",false, "episode 3 - season 4"));

            l->Add(gcnew FilenameProcessorRE(true, "season (?<s>[0-9]+)\\\\e?(?<e>[0-9]{1,3}) ?-",true, "Show Season 3\\E23 - Epname"));
			l->Add(gcnew FilenameProcessorRE(false,"season (?<s>[0-9]+)\\\\episode (?<e>[0-9]{1,3})",true,"Season 3\\Episode 23"));

            return l;
        }

        TVSettings(XmlReader ^reader)
        {
            SetToDefaults();

            reader->Read();
            if (reader->Name != "Settings")
                return; // bail out

            reader->Read();
            while (!reader->EOF)
            {
                if ((reader->Name == "Settings") && !reader->IsStartElement())
                    break; // all done

                if (reader->Name == "Searcher")
                {
                    String ^srch = reader->ReadElementContentAsString(); // and match it based on name...
                    TheSearchers->CurrentSearch = srch;
                }
                else if (reader->Name == "TheSearchers")
                {
                    TheSearchers = gcnew Searchers(reader->ReadSubtree());
                    reader->Read();
                }
                else if (reader->Name == "BGDownload")
                    BGDownload = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "OfflineMode")
                    OfflineMode = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "Replacements")
                {
                    Replacements->Clear();
                    reader->Read();
                    while (!reader->EOF)
                    {
                        if ((reader->Name == "Replacements") && (!reader->IsStartElement()))
                            break;
                        if (reader->Name == "Replace")
                        {
                            Replacements->Add(gcnew Replacement(reader->GetAttribute("This"),reader->GetAttribute("That")));
                            reader->Read();
                        }
                        else
                            reader->ReadOuterXml();
                    }
                    reader->Read();
                }
                else if (reader->Name == "ExportWTWRSS")
                    ExportWTWRSS = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "ExportWTWRSSTo")
                    ExportWTWRSSTo = reader->ReadElementContentAsString();
                else if (reader->Name == "WTWRecentDays")
                    WTWRecentDays = reader->ReadElementContentAsInt();
                else if (reader->Name == "StartupTab")
                    StartupTab = reader->ReadElementContentAsInt();
                else if (reader->Name == "DefaultNamingStyle") // old naming style 
                    NamingStyle->StyleString = CustomName::OldNStyle(reader->ReadElementContentAsInt());
                else if (reader->Name == "NamingStyle")
                    NamingStyle->StyleString = reader->ReadElementContentAsString();
                else if (reader->Name == "NotificationAreaIcon")
                    NotificationAreaIcon = reader->ReadElementContentAsBoolean();
                else if ((reader->Name == "GoodExtensions")||(reader->Name == "VideoExtensions"))
                    SetVideoExtensionsString(reader->ReadElementContentAsString());
                else if (reader->Name == "OtherExtensions")
                    SetOtherExtensionsString(reader->ReadElementContentAsString());
                else if (reader->Name == "ExportRSSMaxDays")
                    ExportRSSMaxDays = reader->ReadElementContentAsInt();
                else if (reader->Name == "ExportRSSMaxShows")
                    ExportRSSMaxShows = reader->ReadElementContentAsInt();
                else if (reader->Name == "KeepTogether")
                    KeepTogether = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "LeadingZeroOnSeason")
                    LeadingZeroOnSeason = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "ShowInTaskbar")
                    ShowInTaskbar = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "RenameTxtToSub")
                    RenameTxtToSub = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "ShowEpisodePictures")
                    ShowEpisodePictures = reader->ReadElementContentAsBoolean();
                else if (reader->Name == "SpecialsFolderName")
                    SpecialsFolderName = reader->ReadElementContentAsString();
				else if (reader->Name == "ExportMissingXML")
					ExportMissingXML = reader->ReadElementContentAsBoolean();
				else if (reader->Name == "ExportMissingXMLTo")
					ExportMissingXMLTo = reader->ReadElementContentAsString();
				else if (reader->Name == "ExportMissingCSV")
					ExportMissingCSV = reader->ReadElementContentAsBoolean();
				else if (reader->Name == "ExportMissingCSVTo")
					ExportMissingCSVTo = reader->ReadElementContentAsString();
				else if (reader->Name == "ExportRenamingXML")
					ExportRenamingXML = reader->ReadElementContentAsBoolean();
				else if (reader->Name == "ExportRenamingXMLTo")
					ExportRenamingXMLTo = reader->ReadElementContentAsString();
				else if (reader->Name == "ExportFOXML")
					ExportFOXML = reader->ReadElementContentAsBoolean();
				else if (reader->Name == "ExportFOXMLTo")
					ExportFOXMLTo = reader->ReadElementContentAsString();
				else if (reader->Name == "ForceLowercaseFilenames")
					ForceLowercaseFilenames = reader->ReadElementContentAsBoolean();
				else if (reader->Name == "IgnoreSamples")
					IgnoreSamples = reader->ReadElementContentAsBoolean();
				else if (reader->Name == "SampleFileMaxSizeMB")
					SampleFileMaxSizeMB = reader->ReadElementContentAsInt();
				else if (reader->Name == "ParallelDownloads")
					ParallelDownloads = reader->ReadElementContentAsInt();
                else if (reader->Name == "FNPRegexs")
                {
                    FNPRegexs->Clear();
                    reader->Read();
                    while (!reader->EOF)
                    {
                        if ((reader->Name == "FNPRegexs") && (!reader->IsStartElement()))
                            break;
                        if (reader->Name == "Regex")
                        {
							String ^s = reader->GetAttribute("Enabled");
							bool en = s != nullptr ? bool::Parse(s) : true;
						 
                            FNPRegexs->Add(gcnew FilenameProcessorRE(en, reader->GetAttribute("RE"),bool::Parse(reader->GetAttribute("UseFullPath")),reader->GetAttribute("Notes")));
                            reader->Read();
                        }
                        else
                            reader->ReadOuterXml();
                    }
                    reader->Read();
                }
                else
                    reader->ReadOuterXml();
            }
        }

        void WriteXML(XmlWriter ^writer)
        {
            writer->WriteStartElement("Settings");
            TheSearchers->WriteXML(writer);
            writer->WriteStartElement("BGDownload");
            writer->WriteValue(BGDownload);
            writer->WriteEndElement();
            writer->WriteStartElement("OfflineMode");
            writer->WriteValue(OfflineMode);
            writer->WriteEndElement();  
            writer->WriteStartElement("Replacements");
            for each (Replacement ^R in Replacements)
            {
                writer->WriteStartElement("Replace");
                writer->WriteStartAttribute("This");
                writer->WriteValue(R->This);
                writer->WriteEndAttribute();  
                writer->WriteStartAttribute("That");
                writer->WriteValue(R->That);
                writer->WriteEndAttribute(); 
                writer->WriteEndElement();
            }
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportWTWRSS");
            writer->WriteValue(ExportWTWRSS);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportWTWRSSTo");
            writer->WriteValue(ExportWTWRSSTo);
            writer->WriteEndElement();  
            writer->WriteStartElement("WTWRecentDays");
            writer->WriteValue(WTWRecentDays);
            writer->WriteEndElement();  
			writer->WriteStartElement("ExportMissingXML");
            writer->WriteValue(ExportMissingXML);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportMissingXMLTo");
            writer->WriteValue(ExportMissingXMLTo);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportMissingCSV");
            writer->WriteValue(ExportMissingCSV);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportMissingCSVTo");
            writer->WriteValue(ExportMissingCSVTo);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportRenamingXML");
            writer->WriteValue(ExportRenamingXML);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportRenamingXMLTo");
            writer->WriteValue(ExportRenamingXMLTo);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportFOXML");
            writer->WriteValue(ExportFOXML);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportFOXMLTo");
            writer->WriteValue(ExportFOXMLTo);
            writer->WriteEndElement();  
            writer->WriteStartElement("StartupTab");
            writer->WriteValue(StartupTab);
            writer->WriteEndElement();  
            writer->WriteStartElement("NamingStyle");
            writer->WriteValue(NamingStyle->StyleString);
            writer->WriteEndElement();  
            writer->WriteStartElement("NotificationAreaIcon");
            writer->WriteValue(NotificationAreaIcon);
            writer->WriteEndElement();  
            writer->WriteStartElement("VideoExtensions");
            writer->WriteValue(VideoExtensionsString);
            writer->WriteEndElement();  
            writer->WriteStartElement("OtherExtensions");
            writer->WriteValue(OtherExtensionsString);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportRSSMaxDays");
            writer->WriteValue(ExportRSSMaxDays);
            writer->WriteEndElement();  
            writer->WriteStartElement("ExportRSSMaxShows");
            writer->WriteValue(ExportRSSMaxShows);
            writer->WriteEndElement();  
            writer->WriteStartElement("KeepTogether");
            writer->WriteValue(KeepTogether);
            writer->WriteEndElement();  
            writer->WriteStartElement("LeadingZeroOnSeason");
            writer->WriteValue(LeadingZeroOnSeason);
            writer->WriteEndElement();  
            writer->WriteStartElement("ShowInTaskbar");
            writer->WriteValue(ShowInTaskbar);
            writer->WriteEndElement();
            writer->WriteStartElement("IgnoreSamples");
            writer->WriteValue(IgnoreSamples);
            writer->WriteEndElement();
            writer->WriteStartElement("ForceLowercaseFilenames");
            writer->WriteValue(ForceLowercaseFilenames);
            writer->WriteEndElement();
            writer->WriteStartElement("RenameTxtToSub");
            writer->WriteValue(RenameTxtToSub);
            writer->WriteEndElement();            
            writer->WriteStartElement("ParallelDownloads");
            writer->WriteValue(ParallelDownloads);
            writer->WriteEndElement();            
            writer->WriteStartElement("ShowEpisodePictures");
            writer->WriteValue(ShowEpisodePictures);
            writer->WriteEndElement();            
            writer->WriteStartElement("SpecialsFolderName");
            writer->WriteValue(SpecialsFolderName);
            writer->WriteEndElement();
            writer->WriteStartElement("FNPRegexs");
            for each (FilenameProcessorRE ^re in FNPRegexs)
            {
                writer->WriteStartElement("Regex");
                writer->WriteStartAttribute("Enabled");
                writer->WriteValue(re->Enabled);
                writer->WriteEndAttribute();
                writer->WriteStartAttribute("RE");
                writer->WriteValue(re->RE);
                writer->WriteEndAttribute();
                writer->WriteStartAttribute("UseFullPath");
                writer->WriteValue(re->UseFullPath);
                writer->WriteEndAttribute();
                writer->WriteStartAttribute("Notes");
                writer->WriteValue(re->Notes);
                writer->WriteEndAttribute();
                writer->WriteEndElement();  // Regex
            }
            writer->WriteEndElement();  // FNPRegexs

            writer->WriteEndElement(); // settings
        }

        public: bool UsefulExtension(String ^sn, bool otherExtensionsToo)
        {
            for each (String ^s in VideoExtensionsArray)
                if (sn->ToLower() == s)
                    return true;
			if (otherExtensionsToo)
				for each (String ^s in OtherExtensionsArray)
					if (sn->ToLower() == s)
						return true;

            return false;
        }
    public: String ^BTSearchURL(ProcessedEpisode ^epi);
    };

} // namespace