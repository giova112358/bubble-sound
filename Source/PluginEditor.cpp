/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BubbleModelAudioProcessorEditor::BubbleModelAudioProcessorEditor (BubbleModelAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), mainPanel(p)
{
    addAndMakeVisible(mainPanel);

    presButton = std::make_unique<juce::TextButton>("Presets");
    addAndMakeVisible(presButton.get());
    presButton->addListener(this);

    setSize (500, 230);
}

BubbleModelAudioProcessorEditor::~BubbleModelAudioProcessorEditor()
{
}

//==============================================================================
void BubbleModelAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto textBounds = bounds.removeFromTop(40);


    g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillRect(bounds);

    g.setColour(juce::Colours::black);
    g.fillRect(textBounds);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f).italicised().withExtraKerningFactor(0.1f));
    g.drawFittedText("SDT Impact", textBounds, juce::Justification::centredLeft, 1);

    juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
}

void BubbleModelAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto rectTop = bounds.removeFromTop(40);
    bounds.reduce(30, 30);

    rectTop.reduce(10, 0);
    presButton->setBounds(rectTop.removeFromRight(120).withSizeKeepingCentre(120, 24));

    auto panelWidth = bounds.getWidth() / 4;
    bounds.removeFromTop(10);

    mainPanel.setBounds(bounds);
}

//================================================================================
void BubbleModelAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    juce::PopupMenu m;
    m.addItem(1, "Save Preset", true, false);
    m.addItem(2, "Load Preset", true, false);
    m.addSeparator();
    if (button == presButton.get()) {
        juce::File path = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile).getParentDirectory();
        int n = path.getNumberOfChildFiles(juce::File::TypesOfFileToFind::findFiles, "*.settings");
        /*DBG(n);*/
        std::vector<juce::String> files;
        if (n > 0) {
            for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(path, false))
                if (entry.getFile().getFileName().contains(".settings"))
                    files.push_back(entry.getFile().getFileName());


            //get number of saved file presets
            for (int i = 0; i < n; i++) {
                m.addItem(i + 3, files[i], true, false);
            }
        }

        auto result = m.showAt(presButton.get());

        if (result == 1) {
            /*juce::String fileSpec = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName()
                + ("D:/presets");
            juce::File file(fileSpec);
            auto rr = file.create();
            juce::XmlElement xmlState = audioProcessor.getAndSavePresetStateValueTree();
            xmlState.writeToFile(file, "");*/

            juce::FileChooser saveChooser("Select a file to save presets", juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile)
                .getParentDirectory()
                , "*.settings");
            if (saveChooser.browseForFileToSave(true)) {
                auto file = saveChooser.getResult();
                //file.appendText(juce::String("WRITE HERE"));
                juce::XmlElement xmlState = audioProcessor.getAndSavePresetStateValueTree();
                xmlState.writeTo(file);
            }
        }
        else if (result == 2) {
            juce::FileChooser loadChooser("Select the preset to load", juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile)
                .getParentDirectory(), "*.settings");
            if (loadChooser.browseForFileToOpen()) {
                auto file2 = loadChooser.getResult();
                juce::XmlDocument xmlDoc(file2);
                std::unique_ptr<juce::XmlElement> xml = xmlDoc.getDocumentElement();
                /*audioProcessor.setPresetStateValueTree(xmlDoc.getDocumentElement());*/
                juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
                audioProcessor.apvts.replaceState(copyState);
            }
        }
        else {
            if (n > 0) {
                if (result >= 3 && result < 3 + n) {
                    juce::File filen = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile)
                        .getParentDirectory().getChildFile(files[result - 3]);/* + "\\"+ files[result - 3]*/;
                    juce::XmlDocument xmlDoc(filen);
                    std::unique_ptr<juce::XmlElement> xml = xmlDoc.getDocumentElement();
                    /*audioProcessor.setPresetStateValueTree(xmlDoc.getDocumentElement());*/
                    juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
                    audioProcessor.apvts.replaceState(copyState);
                }
            }
        }
        //    juce::File path("D:/Sound and Audio Engineering/TESI/repo/Source/Resources");
        //    int n = path.getNumberOfChildFiles(juce::File::TypesOfFileToFind::findFiles, "*.xml");

        //    std::vector<juce::File> files;
        //    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(path, false))
        //        if(entry.getFile().getFileName().contains(".xml"))
        //            files.push_back(entry.getFile());

        //    juce::PopupMenu l;
        //    //get number of saved file presets
        //    for (int i = 0; i < n; i++) {
        //        l.addItem(1, /*"Preset " + juce::String(i)*/files[i].getFileName(), true, loadCurrentPres = 0);
        //    }
        //    /*l.addItem(1, "l1", true, loadCurrentPres = 1);
        //    l.addItem(1, "l2", true, loadCurrentPres = 2);*/
        //    /*auto loadResult = l.showAt(presButton.get());*/
        //    auto loadResult = l.show();
        //    juce::XmlDocument xmlDoc(files[loadResult]);
        //    audioProcessor.setPresetStateValueTree(xmlDoc.getDocumentElement());


    }
}
