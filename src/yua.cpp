#include "yua.h"

#define H0 std::cout <<
#define H1 << std::endl
void displayHelp()
{
        H0 "command line options:"                                          H1;
        H0 "    --input=\"/path/to/your/input.avi\""                        H1;
        H0 "    --append=\"/path/to/additional/input.avi\""                 H1;
        H0 "        can be used multiple times"                             H1;
        H0 "    --output-basename"                                          H1;
        H0 "        e.g.: --output-basename=\"MyRun\""                      H1;
        H0 "        _HQ, _IQ labels and .mp4 extension will be appended automatically" H1;
        H0 "    --output"                                                   H1;
        H0 "        output directory where the converted videos will be saved" H1;
        H0 "    --qualities"                                                H1;
        H0 "        possible values: x i h m l"                             H1;
        H0 "        e.g.: --qualities=mh"                                   H1;
#ifdef WITH_NNEDI3
        H0 "    --interlaced"                                               H1;
#endif
        H0 "    --progressive"                                              H1;
        H0 "    --f"                                                        H1;
        H0 "        e.g.: --f1, --f2, etc."                                 H1;
        H0 "    --d"                                                        H1;
        H0 "        e.g.: --d4, --d1"                                       H1;
        H0 "    --2d"                                                       H1;
        H0 "    --3d"                                                       H1;
        H0 "    --mono"                                                     H1;
        H0 "        corresponds to \"downmix to mono\" option in the gui"   H1;
        H0 "    --statid"                                                   H1;
        H0 "        enables the statid checkbox"                            H1;
        H0 "    --statid#"                                                  H1;
        H0 "        e.g.: --statid1=\"My Name\" --statid2=\"Metroid Prime\" --statid3=\"Single-segment [0:49]\"" H1;
#ifdef WITH_NNEDI3
        H0 "    --bff"                                                      H1;
        H0 "        bottom field first (for interlaced input)"              H1;
        H0 "    --tff"                                                      H1;
        H0 "        top field first (for interlaced input)"                 H1;
#endif
        H0 "    --standard"                                                 H1;
        H0 "        force 4:3 aspect ratio"                                 H1;
        H0 "    --widescreen"                                               H1;
        H0 "        force 16:9 aspect ratio"                                H1;
        H0 ""                                                               H1;
        H0 "    interlaced d4 choices:"                                     H1;
        H0 "    --1-pixel-shift"                                            H1;
        H0 "    --alternate-1-pixel-shift"                                  H1;
        H0 "    --de-deflicker"                                             H1;
        H0 "    --alternate-de-deflicker"                                   H1;
        H0 ""                                                               H1;
        H0 "    --shutdown"                                                 H1;
        H0 "        shut down the computer when finished encoding"          H1;
        H0 "    --trim"                                                     H1;
        H0 "        e.g. --trim=123,1099"                                   H1;
        H0 "        trims based on frame number (not including any statid)" H1;
        H0 ""                                                               H1;
        H0 "    --help / -h"                                                H1;
        H0 "        show this usage information"                            H1;
        H0 "    --version / -v"                                             H1;
        H0 "        display program version number"                         H1;
    std::exit(0);
}
#undef H0
#undef H1

void displayVersion()
{
        // YUA_VERSION is defined in the .pro file
        std::cout << YUA_VERSION << std::endl;
        std::exit(0);
}

Yua::Yua(QWidget *parent)
    : QMainWindow(parent)
    ,currently_encoding(false)
    ,audio_finished(false)
    ,video_finished(false)
    ,muxer_process(this)
    ,progress_bar(this)
    ,mp4box_regex_1("Importing ISO File:.*\\((\\d+)/\\d+\\)")
    ,mp4box_regex_2("ISO File Writing:.*\\((\\d+)/\\d+\\)")
    ,native_width(0)
    ,native_height(0)
    ,slider_resume_value(-9999)
    ,force_field_dominance(false)
    ,cropping_rect(320/2, 240/2, 320, 240)
    ,cropping_rect_set_from_size(640, 480)
    ,crop_moving_left(false)
    ,crop_moving_right(false)
    ,crop_moving_top(false)
    ,crop_moving_bottom(false)
    ,ffmpeg_process(this)
    ,audio_commentary_muxer_process(this)
    #if QT_VERSION >= 0x050000
    ,desktop_path_string(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation))
    #else
    ,desktop_path_string(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation))
    #endif
    ,xq_bitrate_kbit(10000)
{
        setAcceptDrops(true);
        setWindowTitle("Yua");
        setWindowIcon(QIcon(":/yua_64x64.png"));





        QCoreApplication::setOrganizationName("Speed Demos Archive");
        QCoreApplication::setOrganizationDomain("speeddemosarchive.com");
        QCoreApplication::setApplicationName("Yua");
        QCoreApplication::setApplicationVersion(QString("%1").arg(YUA_VERSION));
        settings = new QSettings;




        display_timestamp_datetime.setUtcOffset(0);



#ifdef Q_OS_MAC
        QMenuBar *menu_bar = new QMenuBar(0);
#else
        QMenuBar *menu_bar = menuBar();
#endif

        file_menu = menu_bar->addMenu(tr("&File"));

        QAction *open_action = file_menu->addAction(tr("&Open ..."));
        open_action->setShortcut(QKeySequence::Open);
        //"triggered" == the user did it, "toggled" == either the user did it or we did it programmatically
        connect(open_action, SIGNAL(triggered()), this, SLOT(open_dialog()));

        append_action = file_menu->addAction(tr("&Append ..."));
        append_action->setShortcut(tr("Ctrl++"));
        connect(append_action, SIGNAL(triggered()), this, SLOT(append_dialog()));
        append_action->setDisabled(true);

        file_menu->addSeparator();

        save_settings_action = file_menu->addAction(tr("&Save settings ..."));
        save_settings_action->setShortcut(tr("Ctrl+S"));
        connect(save_settings_action, SIGNAL(triggered()), this, SLOT(save_settings()));

        load_settings_action = file_menu->addAction(tr("&Load settings ..."));
        load_settings_action->setShortcut(tr("Ctrl+L"));
        connect(load_settings_action, SIGNAL(triggered()), this, SLOT(load_settings()));

        file_menu->addSeparator();

        encode_action = file_menu->addAction(tr("&Encode"));
        encode_action->setShortcut(tr("Ctrl+E"));
        connect(encode_action, SIGNAL(triggered()), this, SLOT(encoding_start()));

        file_menu->addSeparator();

        QAction *exit_action = file_menu->addAction(tr("E&xit"));
        exit_action->setShortcut(tr("Ctrl+Q"));
        connect(exit_action, SIGNAL(triggered()), this, SLOT(exit_yua()));


        trim_menu = menu_bar->addMenu(tr("&Trim"));

        QAction *go_to_frame_action = trim_menu->addAction(tr("Go to &frame ..."));
        go_to_frame_action->setShortcut(tr("Ctrl+G"));
        connect(go_to_frame_action, SIGNAL(triggered()), this, SLOT(go_to_frame_dialog()));

        trim_menu->addSeparator();

        QAction *trim_start_frame_action = trim_menu->addAction(tr("Set &start frame"));
        QAction *trim_end_frame_action = trim_menu->addAction(tr("Set &end frame"));
        trim_start_frame_action->setShortcut(tr("home"));
        trim_end_frame_action->setShortcut(tr("end"));
        connect(trim_start_frame_action, SIGNAL(triggered()), this, SLOT(trim_set_start_frame()));
        connect(trim_end_frame_action, SIGNAL(triggered()), this, SLOT(trim_set_end_frame()));

        trim_menu->addSeparator();

        QAction *go_to_start_frame_action = trim_menu->addAction(tr("Go to start frame"));
        QAction *go_to_end_frame_action = trim_menu->addAction(tr("Go to end frame"));
        go_to_start_frame_action->setShortcut(tr("Ctrl+home"));
        go_to_end_frame_action->setShortcut(tr("Ctrl+end"));
        connect(go_to_start_frame_action, SIGNAL(triggered()), this, SLOT(go_to_trim_start_frame()));
        connect(go_to_end_frame_action, SIGNAL(triggered()), this, SLOT(go_to_trim_end_frame()));

        trim_menu->addSeparator();

        QAction *delete_selection_action = trim_menu->addAction(tr("Delete selection"));
        delete_selection_action->setShortcut(QKeySequence::Delete);
        connect(delete_selection_action, SIGNAL(triggered()), this, SLOT(delete_selection()));
        slider.addAction(delete_selection_action);



        screenshot_menu = menu_bar->addMenu(tr("&Screenshot"));

        QAction *take_screenshot_action = screenshot_menu->addAction(tr("Take &screenshot"));
        take_screenshot_action->setShortcut(tr("Ctrl+Shift+S"));
        connect(take_screenshot_action, SIGNAL(triggered()), this, SLOT(screenshot_save()));



        tools_menu = menu_bar->addMenu(tr("T&ools"));

        QAction *add_audio_commentary_action = tools_menu->addAction(tr("Add &audio commentary ..."));
        add_audio_commentary_action->setShortcut(tr("Ctrl+Shift+A"));
        connect(add_audio_commentary_action, SIGNAL(triggered()), this, SLOT(add_audio_commentary()));



        if (0) { //for debugging the Power_Management class under the various operating systems (20130503)
                QMenu *power_management_menu = menu_bar->addMenu(tr("&PM"));

                shut_down_action = power_management_menu->addAction(tr("&Shut down"));
                reboot_action = power_management_menu->addAction(tr("&Reboot"));
                sleep_action = power_management_menu->addAction(tr("S&leep"));
                hibernate_action = power_management_menu->addAction(tr("&Hibernate"));
                lock_action = power_management_menu->addAction(tr("&Lock"));
                connect(shut_down_action, SIGNAL(triggered()), this, SLOT(power_management_slot()));
                connect(reboot_action, SIGNAL(triggered()), this, SLOT(power_management_slot()));
                connect(sleep_action, SIGNAL(triggered()), this, SLOT(power_management_slot()));
                connect(hibernate_action, SIGNAL(triggered()), this, SLOT(power_management_slot()));
                connect(lock_action, SIGNAL(triggered()), this, SLOT(power_management_slot()));
        }



        help_menu = menu_bar->addMenu(tr("&Help"));
        QAction *about_yua_action = help_menu->addAction(tr("About &Yua"));
        connect(about_yua_action, SIGNAL(triggered()), this, SLOT(about()));
        about_yua_action->setMenuRole(QAction::AboutRole);
        QAction *about_qt_action = help_menu->addAction(tr("About &Qt"));
        connect(about_qt_action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
        about_qt_action->setMenuRole(QAction::AboutQtRole);





        QGroupBox *interlaced_group_box = new QGroupBox(tr("Input type"));
        QVBoxLayout *interlaced_layout = new QVBoxLayout;
        interlaced_group_box->setLayout(interlaced_layout);
        interlaced_group_box->setToolTip(tr("Interlaced video displays \"combing\" artifacts (horizontal lines) and usually comes from analog video capture of old (Standard Definition) video games."));
        progressive_button = new QRadioButton(tr("Progressive scan"));
        interlaced_button = new QRadioButton(tr("Interlaced"));
        interlaced_layout->addWidget(progressive_button);
#ifdef WITH_NNEDI3
        interlaced_layout->addWidget(interlaced_button);
#endif
        progressive_button->setChecked(true);
        connect(progressive_button, SIGNAL(clicked()), this, SLOT(interlaced_changed()));
        connect(interlaced_button, SIGNAL(clicked()), this, SLOT(interlaced_changed()));

        field_dominance_group_box = new QGroupBox(tr("Field dominance"));
        QVBoxLayout *field_dominance_layout = new QVBoxLayout;
        field_dominance_group_box->setLayout(field_dominance_layout);
        field_dominance_group_box->setToolTip(tr("In some cases, you must pick the correct field dominance, or the video will not play smoothly (it will jerk back and forth as you step through each frame)."));
        tff_button = new QRadioButton(tr("Top field first"));
        bff_button = new QRadioButton(tr("Bottom field first"));
#ifdef WITH_NNEDI3
        field_dominance_layout->addWidget(tff_button);
        field_dominance_layout->addWidget(bff_button);
#endif
        bff_button->setChecked(true);
        connect(tff_button, SIGNAL(clicked()), this, SLOT(field_dominance_changed()));
        connect(bff_button, SIGNAL(clicked()), this, SLOT(field_dominance_changed()));

        aspect_ratio_group_box = new QGroupBox(tr("Aspect ratio"));
        aspect_ratio_group_box->setCheckable(true);
        connect(aspect_ratio_group_box, SIGNAL(clicked()), this, SLOT(aspect_ratio_changed()));
        QVBoxLayout *aspect_ratio_layout = new QVBoxLayout;
        aspect_ratio_group_box->setLayout(aspect_ratio_layout);
        aspect_ratio_group_box->setToolTip(tr("Old stuff is all 4:3 (Standard aspect ratio). A lot of newer stuff (HDTV) is 16:9 (Widescreen)."));
        standard_button = new QRadioButton(tr("Standard"));
        widescreen_button = new QRadioButton(tr("Widescreen"));
        aspect_ratio_layout->addWidget(standard_button);
        aspect_ratio_layout->addWidget(widescreen_button);
        standard_button->setChecked(true);
        connect(standard_button, SIGNAL(clicked()), this, SLOT(aspect_ratio_changed()));
        connect(widescreen_button, SIGNAL(clicked()), this, SLOT(aspect_ratio_changed()));

        QGroupBox *d_group_box = new QGroupBox(tr("Dimensions"));
        QVBoxLayout *d_layout = new QVBoxLayout;
        d_group_box->setLayout(d_layout);
        d_group_box->setToolTip(tr("In general, games for systems released before the Dreamcast in 1999 are probably D4. Dreamcast and later are probably D1. All GBA games are D4. If in doubt, post a sample on the forum and we will tell you."));
        d4_button = new QRadioButton("D4");
        d1_button = new QRadioButton("D1");
        d_layout->addWidget(d4_button);
        d_layout->addWidget(d1_button);
        d1_button->setChecked(true);
        connect(d4_button, SIGNAL(clicked()), this, SLOT(d_changed()));
        connect(d1_button, SIGNAL(clicked()), this, SLOT(d_changed()));

        QGroupBox *f_group_box = new QGroupBox(tr("Framerate"));
        QVBoxLayout *f_layout = new QVBoxLayout;
        f_group_box->setLayout(f_layout);
        f_group_box->setToolTip(tr("If every frame is different, choose F1. If every other frame is different, choose F2. If every third frame is different, choose F3."));
        f1_button = new QRadioButton("F1");
        f2_button = new QRadioButton("F2");
        f3_button = new QRadioButton("F3");
        f_layout->addWidget(f1_button);
        f_layout->addWidget(f2_button);
        f_layout->addWidget(f3_button);
        f1_button->setChecked(true);
        connect(f1_button, SIGNAL(clicked()), this, SLOT(f_changed()));
        connect(f2_button, SIGNAL(clicked()), this, SLOT(f_changed()));
        connect(f3_button, SIGNAL(clicked()), this, SLOT(f_changed()));

        QGroupBox *fdp_group_box = new QGroupBox(tr("Decimation"));
        QVBoxLayout *fdp_layout = new QVBoxLayout;
        fdp_group_box->setLayout(fdp_layout);
        fdp_group_box->setToolTip(tr("If this is a very old-style (8-bit or 16-bit) game, choose 2D. Otherwise, choose 3D."));
        fdp_2d_button = new QRadioButton("2D");
        fdp_3d_button = new QRadioButton("3D");
        fdp_layout->addWidget(fdp_2d_button);
        fdp_layout->addWidget(fdp_3d_button);
        fdp_3d_button->setChecked(true);


        QGroupBox *misc_group_box = new QGroupBox(tr("Misc"));
        QVBoxLayout *misc_layout = new QVBoxLayout;
        misc_group_box->setLayout(misc_layout);

        crop_checkbox = new QCheckBox(tr("Crop"));
        crop_checkbox->setToolTip(tr("Remove the sides of the picture. Use this for e.g. Game Boy Player material to crop off the border."));
        connect(crop_checkbox, SIGNAL(clicked(bool)), this, SLOT(crop_checkbox_clicked(bool)));
        misc_layout->addWidget(crop_checkbox);

        mono_checkbox = new QCheckBox(tr("Downmix to mono"));
        mono_checkbox->setToolTip(tr("Use this if the game only produces monaural audio, e.g. all NES and Sega Master System games."));
        misc_layout->addWidget(mono_checkbox);

        shut_down_when_done_checkbox = new QCheckBox(tr("Shut down when done"));
        shut_down_when_done_checkbox->setToolTip(tr("Yua will shut down the computer when encoding is finished."));
        misc_layout->addWidget(shut_down_when_done_checkbox);


        QVBoxLayout *qualities_layout_rows = new QVBoxLayout;
        QHBoxLayout *qualities_layout_outer = new QHBoxLayout;
        qualities_layout_rows->addLayout(qualities_layout_outer);
        QLabel *qualities_recommended_label = new QLabel(tr("(<font color=red>required</font>)"));
        qualities_recommended_label->setAlignment(Qt::AlignCenter);
        qualities_layout_rows->addWidget(qualities_recommended_label);

        QGroupBox *qualities_group_box = new QGroupBox(tr("Output"));
        qualities_group_box->setLayout(qualities_layout_rows);
        qualities_group_box->setToolTip(tr("HQ, MQ, and LQ are required for all submissions to SDA. IQ is required for D1 F1 videos. XQ is required for HD videos."));
        QVBoxLayout *qualities_layout_inner_1 = new QVBoxLayout;
        QVBoxLayout *qualities_layout_inner_2 = new QVBoxLayout;
        qualities_layout_outer->addLayout(qualities_layout_inner_1);
        qualities_layout_outer->addLayout(qualities_layout_inner_2);
        lq_button = new QCheckBox("LQ");
        mq_button = new QCheckBox("MQ");
        hq_button = new QCheckBox("HQ");
        iq_button = new QCheckBox("IQ");
        xq_button = new QCheckBox("XQ");
        lq_button->setToolTip(tr("Low quality"));
        mq_button->setToolTip(tr("Normal (Medium) quality"));
        hq_button->setToolTip(tr("High quality"));
        iq_button->setToolTip(tr("Insane quality"));
        xq_button->setToolTip(tr("X-Treme quality"));
        qualities_layout_inner_1->addWidget(lq_button);
        qualities_layout_inner_1->addWidget(mq_button);
        qualities_layout_inner_1->addWidget(hq_button);
        qualities_layout_inner_2->addWidget(iq_button);
        qualities_layout_inner_2->addWidget(xq_button);


        statid_group_box = new QGroupBox(tr("StatID"));
        QVBoxLayout *statid_layout = new QVBoxLayout;
        statid_group_box->setLayout(statid_layout);
        statid_group_box->setToolTip(tr("Adding a so-called Station ID to the front and end of your run will discourage honest individuals from stealing the video. It can also help promote SDA."));
        statid_group_box->setCheckable(true);
        connect(statid_group_box, SIGNAL(toggled(bool)), this, SLOT(statid_toggled(bool)));
        statid_line_1 = new QLineEdit;
        statid_line_2 = new QLineEdit;
        statid_line_3 = new QLineEdit;
        sda_logo_checkbox = new QCheckBox(tr("SDA logo"));
        audio_commentary_checkbox = new QCheckBox(tr("Audio commentary"));
        statid_layout->addWidget(statid_line_1);
        statid_layout->addWidget(statid_line_2);
        statid_layout->addWidget(statid_line_3);
        statid_layout->addWidget(sda_logo_checkbox);
        statid_layout->addWidget(audio_commentary_checkbox);
        //textEdited() == the user did it; textChanged() == the user did it or it was done programmatically
        connect(statid_line_1, SIGNAL(textEdited(QString)), this, SLOT(statid_changed()));
        connect(statid_line_2, SIGNAL(textEdited(QString)), this, SLOT(statid_changed()));
        connect(statid_line_3, SIGNAL(textEdited(QString)), this, SLOT(statid_changed()));
        connect(sda_logo_checkbox, SIGNAL(toggled(bool)), this, SLOT(statid_changed()));
        connect(audio_commentary_checkbox, SIGNAL(toggled(bool)), this, SLOT(statid_changed()));
        statid_group_box->setChecked(false);

        output_path_group_box = new QGroupBox(tr("Output folder/filename"));
        QVBoxLayout *output_path_layout = new QVBoxLayout;
        output_path_group_box->setLayout(output_path_layout);
        output_path_current_label = new QLabel;
        QPushButton *output_directory_choose_button = new QPushButton(tr("Change ..."));
        connect(output_directory_choose_button, SIGNAL(clicked()), this, SLOT(output_directory_choose()));
        output_name_edit = new QLineEdit;
        output_path_layout->addWidget(output_path_current_label);
        output_path_layout->addWidget(output_directory_choose_button);
        output_path_layout->addWidget(output_name_edit);





        QButtonGroup *d4_image_processing_button_group = new QButtonGroup;

        no_change_button = new QRadioButton("No change");
        one_pixel_bob_button = new QRadioButton("1-pixel shift");
        alternate_one_pixel_bob_button = new QRadioButton("Alternate 1-pixel shift");
        retard_bob_button = new QRadioButton("De-deflicker");
        alternate_retard_bob_button = new QRadioButton("Alternate de-deflicker");
        no_change_button->setChecked(true);

        d4_image_processing_button_group->addButton(no_change_button);
        d4_image_processing_button_group->addButton(one_pixel_bob_button);
        d4_image_processing_button_group->addButton(alternate_one_pixel_bob_button);
        d4_image_processing_button_group->addButton(retard_bob_button);
        d4_image_processing_button_group->addButton(alternate_retard_bob_button);


        progress_bar.setRange(0, 10000);
        progress_bar.setTextVisible(false);

        stop_button = new QPushButton(tr("Abort"));
        connect(stop_button, SIGNAL(clicked()), this, SLOT(stop_button_pressed()));
        stop_button->setDisabled(true);

        slider.setOrientation(Qt::Horizontal);
        slider.setFocusPolicy(Qt::StrongFocus);
        slider.setTracking(true);

        trimpoints_label.setScaledContents(true);
        trimpoints_label.setMinimumWidth(1);
        trimpoints_label.setMinimumHeight(10);
        trimpoints_label.resize(1,1);

        status_textedit.setReadOnly(true);
        status_textedit.setMaximumHeight(100);

        field_dominance_group_box->hide();






        QHBoxLayout *layout = new QHBoxLayout;
        layout->setSpacing(0);
//        layout->setContentsMargins(0,0,0,0);
        QWidget *dummy_widget = new QWidget;
        dummy_widget->setLayout(layout);
        dummy_widget->setContentsMargins(0,0,0,0);
        setCentralWidget(dummy_widget);


        QGridLayout *d4_filters_layout = new QGridLayout;
        QVBoxLayout *main_layout = new QVBoxLayout;
        QVBoxLayout *options_layout = new QVBoxLayout;
        QVBoxLayout *options_layout_2 = new QVBoxLayout;

        QHBoxLayout *d4_options_and_main_display_layout = new QHBoxLayout;

        QVBoxLayout *display_and_bars_layout = new QVBoxLayout;
        QHBoxLayout *progress_bar_and_stop_button_layout = new QHBoxLayout;
        display_and_bars_layout->addWidget(&pretty_video_info_label);
        display_and_bars_layout->addWidget(&pretty_audio_info_label);
        display_and_bars_layout->addLayout(d4_options_and_main_display_layout, 1);
        display_and_bars_layout->addWidget(&slider);
        display_and_bars_layout->addWidget(&trimpoints_label);
        display_and_bars_layout->addWidget(&status_textedit);
        display_and_bars_layout->addLayout(progress_bar_and_stop_button_layout);
        progress_bar_and_stop_button_layout->addWidget(&progress_bar);
        progress_bar_and_stop_button_layout->addSpacing(16);
        progress_bar_and_stop_button_layout->addWidget(stop_button);

        main_layout->addWidget(&preview_display, 1);
        main_layout->addWidget(&no_change_display, 1);
        no_change_display.setMaximumWidth(320);

        d4_options_container_widget = new QWidget; //so we can hide all the d4 stuff all at once
        d4_options_container_widget->setContentsMargins(0,0,0,0);
        d4_filters_layout->setContentsMargins(0,0,0,0);
        d4_filters_layout->setSpacing(0);
        d4_options_container_widget->setMinimumWidth(320*2);
        d4_options_container_widget->setMinimumHeight(240*2);
        d4_options_container_widget->setLayout(d4_filters_layout);
        d4_options_container_widget->hide();

        d4_options_and_main_display_layout->addWidget(d4_options_container_widget);
        d4_options_and_main_display_layout->addLayout(main_layout, 1);

        layout->addLayout(display_and_bars_layout, 1);
        layout->addLayout(options_layout);
        layout->addSpacing(10);
        layout->addLayout(options_layout_2);

        d4_filters_layout->addWidget(&one_pixel_bob_display, 0, 0);
        d4_filters_layout->addWidget(&alternate_one_pixel_bob_display, 0, 1);
        d4_filters_layout->addWidget(&retard_bob_display, 1, 0);
        d4_filters_layout->addWidget(&alternate_retard_bob_display, 1, 1);

        connect(&no_change_display, SIGNAL(clicked()), this, SLOT(d4_filter_display_clicked()));
        connect(&one_pixel_bob_display, SIGNAL(clicked()), this, SLOT(d4_filter_display_clicked()));
        connect(&alternate_one_pixel_bob_display, SIGNAL(clicked()), this, SLOT(d4_filter_display_clicked()));
        connect(&retard_bob_display, SIGNAL(clicked()), this, SLOT(d4_filter_display_clicked()));
        connect(&alternate_retard_bob_display, SIGNAL(clicked()), this, SLOT(d4_filter_display_clicked()));


        preview_display.setMinimumWidth(320);
        no_change_display.setMinimumWidth(320);


        options_layout->addWidget(interlaced_group_box);
        options_layout->addWidget(field_dominance_group_box);
        options_layout->addWidget(aspect_ratio_group_box);
        options_layout->addWidget(d_group_box);
        options_layout->addWidget(f_group_box);
        options_layout->addWidget(fdp_group_box);
        options_layout->addStretch(1);

        options_layout_2->addWidget(misc_group_box);
        options_layout_2->addWidget(qualities_group_box);
        options_layout_2->addSpacing(10);
        options_layout_2->addWidget(statid_group_box);
        options_layout_2->addWidget(output_path_group_box);
        options_layout_2->addStretch(1);









        //for vate_ui()
        buttons
#ifdef WITH_NNEDI3
                        << tff_button << bff_button
#endif
                        << standard_button << widescreen_button
                        << progressive_button << interlaced_button
                        << no_change_button << one_pixel_bob_button << alternate_one_pixel_bob_button << retard_bob_button << alternate_retard_bob_button
                        << f1_button << f2_button << f3_button
                        << fdp_2d_button << fdp_3d_button
                        << d4_button << d1_button
                           ;
        checkboxes
                        << xq_button
                        << iq_button
                        << hq_button
                        << mq_button
                        << lq_button
                        << crop_checkbox
                        << mono_checkbox
                           ;
        actions
                        << open_action
                        << append_action
                        << encode_action
                        << save_settings_action
                        << load_settings_action

                        << go_to_frame_action
                        << trim_start_frame_action
                        << trim_end_frame_action
                        << go_to_start_frame_action
                        << go_to_end_frame_action
                        << delete_selection_action

                        << take_screenshot_action
                           ;










//        index_progress_bar.setRange(0, 10000);
//        index_progress_bar.setTextVisible(false);
//        index_progress_widget = new QDialog(this);
//        index_progress_widget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
//        index_progress_widget->setModal(true);

//        QLabel *index_progress_label = new QLabel(tr("Yua is examining the video ..."));
//        index_progress_label->setAlignment(Qt::AlignHCenter);

//        QVBoxLayout *index_progress_layout = new QVBoxLayout;
//        index_progress_widget->setLayout(index_progress_layout);
//        index_progress_layout->addWidget(index_progress_label);
//        index_progress_layout->addWidget(&index_progress_bar);
//        index_progress_widget->hide();




        decoder.seek_jobs = &seek_jobs;
        video_encoder.mutex = &ffmpeg_mutex;
        audio_encoder.mutex = &ffmpeg_mutex;

        decoder.moveToThread(&decoder_thread);
        deinterlacer.moveToThread(&deinterlacer_thread);
        deinterlacer2.moveToThread(&deinterlacer2_thread);
        audio_encoder.moveToThread(&audio_encoder_thread);
        video_encoder.moveToThread(&video_encoder_thread);



        qRegisterMetaType<Audio_Information>("Audio_Information");
        qRegisterMetaType<Video_Information>("Video_Information");
        qRegisterMetaType<Progress_Type>("Progress_Type");
        qRegisterMetaType<QList<QByteArray> >("QList<QByteArray>");
        qRegisterMetaType<Frame>("Frame");


        connect(this, SIGNAL(open_file(QString)), &decoder, SLOT(open(QString)));
        connect(this, SIGNAL(append_files(QStringList)), &decoder, SLOT(append(QStringList)));


        connect(this, SIGNAL(seek()), &decoder, SLOT(seek()));


        connect(&decoder, SIGNAL(video_frame_out(Frame)), this, SLOT(new_image(Frame)));
        connect(this, SIGNAL(deinterlace(Frame,bool)), &deinterlacer, SLOT(deinterlace(Frame,bool)));
        connect(&deinterlacer, SIGNAL(top_bottom_field_ready(Frame,Frame)), this, SLOT(top_bottom_field(Frame,Frame)));

        connect(this, SIGNAL(push_down_one_line(Frame,Frame)), &deinterlacer, SLOT(push_down_one_line(Frame,Frame)));
        connect(&deinterlacer, SIGNAL(pushed_down(Frame,Frame,Frame,Frame)), this, SLOT(one_pixel_bobbed(Frame,Frame,Frame,Frame)));

        connect(this, SIGNAL(retard_bob(Frame,Frame,bool)), &deinterlacer2, SLOT(retard_bob(Frame,Frame,bool)));
        connect(&deinterlacer2, SIGNAL(retard_bobbed(Frame,Frame,bool)), this, SLOT(retard_bobbed(Frame,Frame,bool)));

        connect(this, SIGNAL(no_change_image_ready(QImage)), &no_change_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(one_pixel_bob_image_ready(QImage)), &one_pixel_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(alternate_one_pixel_bob_image_ready(QImage)), &alternate_one_pixel_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(retard_bob_image_ready(QImage)), &retard_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(alternate_retard_bob_image_ready(QImage)), &alternate_retard_bob_display, SLOT(display_image(QImage)));


        connect(this, SIGNAL(audio_encoding_start(Audio_Information)), &audio_encoder, SLOT(start(Audio_Information)));
        connect(this, SIGNAL(set_encoding_abort()), &decoder, SLOT(set_encoding_abort()));
        connect(&decoder, SIGNAL(decoding_finished(int)), this, SLOT(decoding_finished(int)));
        connect(this, SIGNAL(decoding_start(int,int,int)), &decoder, SLOT(decoding_start(int,int,int)));
        connect(&audio_encoder, SIGNAL(finished(QString)), this, SLOT(audio_encoding_finished(QString)));


        connect(this, SIGNAL(video_encoding_start(Video_Information, QString, int)), &video_encoder, SLOT(start(Video_Information, QString, int)));
        connect(this, SIGNAL(video_encoding_finish(int)), &video_encoder, SLOT(finish(int)));
        connect(this, SIGNAL(audio_encoding_finish()), &audio_encoder, SLOT(finish()));
        connect(this, SIGNAL(queue_video_frame(Frame)), this, SLOT(queue_frame_slot(Frame)));
        connect(this, SIGNAL(encode_frame(Frame)), &video_encoder, SLOT(encode_frame(Frame)));
        connect(this, SIGNAL(set_pass(int)), &video_encoder, SLOT(set_pass(int)));
        connect(&video_encoder, SIGNAL(finished(QString, int)), this, SLOT(video_encoding_finished(QString, int)));
        connect(&video_encoder, SIGNAL(decode_to(int)), &decoder, SLOT(set_decode_to(int)));
        connect(&video_encoder, SIGNAL(add_progress(Progress_Type,double)), this, SLOT(add_progress(Progress_Type,double)));
        connect(this, SIGNAL(set_estimated_total_frames_to_encode(int)), &video_encoder, SLOT(set_estimated_total_frames_to_encode(int)));
        connect(&video_encoder, SIGNAL(request_second_pass()), this, SLOT(restart_decoder()));

        connect(&muxer_process, SIGNAL(readyReadStandardError()), this, SLOT(process_read_stderr()));
        connect(&muxer_process, SIGNAL(readyReadStandardOutput()), this, SLOT(process_read_stdout()));
        connect(&muxer_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(process_error(QProcess::ProcessError)));
        connect(&muxer_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(process_finished(int,QProcess::ExitStatus)));

        connect(&ffmpeg_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(ffmpeg_process_error(QProcess::ProcessError)));
        connect(&ffmpeg_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(ffmpeg_process_finished(int,QProcess::ExitStatus)));

        connect(&audio_commentary_muxer_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(audio_commentary_muxer_process_error(QProcess::ProcessError)));
        connect(&audio_commentary_muxer_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(audio_commentary_muxer_process_finished(int,QProcess::ExitStatus)));

        connect(&decoder, SIGNAL(error_with_string(QString)), this, SLOT(error_slot(QString)));
        connect(&video_encoder, SIGNAL(error_with_string(QString)), this, SLOT(error_slot(QString)));
        connect(&audio_encoder, SIGNAL(error_with_string(QString)), this, SLOT(error_slot(QString)));

        connect(this, SIGNAL(set_video_timebase(qint64,qint64)), &video_encoder, SLOT(set_timebase(qint64,qint64)));

        connect(this, SIGNAL(set_preview_size(int,int)), &deinterlacer, SLOT(set_preview_size(int,int)));
        connect(this, SIGNAL(set_statid_size(int,int)), &decoder, SLOT(set_statid_size(int,int)));
        connect(this, SIGNAL(make_preview(Frame)), &deinterlacer, SLOT(make_preview(Frame)));
        connect(this, SIGNAL(preview_ready(QImage)), &preview_display, SLOT(display_image(QImage)));
        connect(&deinterlacer, SIGNAL(preview_ready(Frame)), this, SLOT(new_preview(Frame)));
        connect(this, SIGNAL(force_interlaced(bool)), &decoder, SLOT(force_interlaced(bool)));

        connect(&decoder, SIGNAL(write_to_log(QString,QString)), this, SLOT(write_to_log(QString,QString)));
        connect(&audio_encoder, SIGNAL(write_to_log(QString,QString)), this, SLOT(write_to_log(QString,QString)));
        connect(&video_encoder, SIGNAL(write_to_log(QString,QString)), this, SLOT(write_to_log(QString,QString)));

        connect(&decoder, SIGNAL(index_begin()), this, SLOT(index_begun()));
        connect(&decoder, SIGNAL(index_finish()), this, SLOT(index_finished()));
        connect(&decoder, SIGNAL(normalize_begin(QString)), this, SLOT(normalize_begun(QString)));
        connect(&decoder, SIGNAL(normalize_finish(QString)), this, SLOT(normalize_finished(QString)));
        connect(&decoder, SIGNAL(index_progress(qint64,qint64)), this, SLOT(index_progress(qint64,qint64)));

        connect(this, SIGNAL(double_height(Frame,bool)), &deinterlacer, SLOT(double_height(Frame,bool)));
        connect(&deinterlacer, SIGNAL(height_doubled(Frame)), this, SLOT(height_doubled(Frame)));

        connect(&decoder, SIGNAL(pretty_info(QString,QString)), this, SLOT(set_pretty_info(QString,QString)));
        connect(&decoder, SIGNAL(successfully_opened(QStringList,Video_Information,bool,Audio_Information)), this, SLOT(open_successful(QStringList,Video_Information,bool,Audio_Information)));

        connect(this, SIGNAL(set_aspect_ratio(int,int)), &video_encoder, SLOT(set_aspect_ratio(int,int)));
        connect(this, SIGNAL(set_aspect_ratio(int,int)), &preview_display, SLOT(set_aspect_ratio(int,int)));

        connect(this, SIGNAL(preview_ready_d4(QImage)), &no_change_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(preview_ready_d4(QImage)), &one_pixel_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(preview_ready_d4(QImage)), &alternate_one_pixel_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(preview_ready_d4(QImage)), &retard_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(preview_ready_d4(QImage)), &alternate_retard_bob_display, SLOT(display_image(QImage)));

        connect(this, SIGNAL(set_audio_source_information(Audio_Information)), &audio_encoder, SLOT(set_source_information(Audio_Information)));
        connect(&decoder, SIGNAL(audio_data_out(QByteArray, qint64)), this, SLOT(audio_data_in(QByteArray, qint64)));
        connect(this, SIGNAL(audio_data_out(QByteArray, qint64)), &audio_encoder, SLOT(encode_samples(QByteArray, qint64)));

        connect(&slider, SIGNAL(valueChanged(int)), this, SLOT(slider_moved(int)));

        connect(this, SIGNAL(set_cli_force(bool,bool)), &decoder, SLOT(set_cli_force(bool,bool)));

        connect(this, SIGNAL(set_append_statid(bool)), &decoder, SLOT(set_append_statid(bool)));
        connect(this, SIGNAL(set_statid_line(int,QString)), &decoder, SIGNAL(set_statid_line(int,QString)));
        connect(this, SIGNAL(set_statid_sda_logo(bool)), &decoder, SIGNAL(set_statid_sda_logo(bool)));
        connect(this, SIGNAL(set_statid_audio_commentary(bool)), &decoder, SIGNAL(set_statid_audio_commentary(bool)));

        connect(this, SIGNAL(delete_between(int,int)), &decoder, SLOT(delete_between(int,int)));

        connect(this, SIGNAL(set_target_number_of_audio_channels(int)), &decoder, SLOT(set_target_number_of_audio_channels(int)));
        connect(this, SIGNAL(set_target_number_of_audio_channels(int)), &audio_encoder, SLOT(set_target_number_of_audio_channels(int)));

        connect(&decoder, SIGNAL(restore_ui()), this, SLOT(restore_ui()));

        connect(&preview_display, SIGNAL(clicked(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&preview_display, SIGNAL(mouse_moved(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));

        connect(&no_change_display, SIGNAL(clicked(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&one_pixel_bob_display, SIGNAL(clicked(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&alternate_one_pixel_bob_display, SIGNAL(clicked(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&retard_bob_display, SIGNAL(clicked(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&alternate_retard_bob_display, SIGNAL(clicked(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));

        connect(&no_change_display, SIGNAL(mouse_moved(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&one_pixel_bob_display, SIGNAL(mouse_moved(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&alternate_one_pixel_bob_display, SIGNAL(mouse_moved(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&retard_bob_display, SIGNAL(mouse_moved(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));
        connect(&alternate_retard_bob_display, SIGNAL(mouse_moved(QMouseEvent*)), this, SLOT(display_clicked(QMouseEvent*)));

        connect(this, SIGNAL(encode_frame_preview(QImage)), &preview_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(encode_frame_preview(QImage)), &no_change_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(encode_frame_preview(QImage)), &one_pixel_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(encode_frame_preview(QImage)), &alternate_one_pixel_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(encode_frame_preview(QImage)), &retard_bob_display, SLOT(display_image(QImage)));
        connect(this, SIGNAL(encode_frame_preview(QImage)), &alternate_retard_bob_display, SLOT(display_image(QImage)));



        decoder_thread.start();
        deinterlacer_thread.start();
        deinterlacer2_thread.start();
        audio_encoder_thread.start();
        video_encoder_thread.start();



        if (!temporary_directory_file.open()) {
                error_slot(tr("Yua: failed to open temporary directory file"));
        }
        temp_dir = temporary_directory_file.fileName() + "_dir";
        QDir temporary_directory(temp_dir);
        temporary_directory.mkpath(".");

        QStringList resource_names;
        resource_names
                        << "mp4box"
                        << "ffmpeg"
                           ;

        QString resource_ext = "";
#ifdef Q_OS_WIN32
        resource_ext = ".exe";
#endif

        foreach (QString resource_name, resource_names) {
                QString resource_outpath = temp_dir + "/" + resource_name + resource_ext;
                resource_outpaths << resource_outpath;
                resource_pathname[resource_name] = resource_outpath;

                QFile resource_out_file(resource_outpath);
                QFile resource_in_file(":/" + resource_name);
                if (!resource_in_file.exists()) {
                                int resource_part = 1;
                                resource_in_file.setFileName(QString(":/%1.%2").arg(resource_name).arg(resource_part));
                                if (!resource_in_file.exists()) {
                                        qDebug() << "Yua: resource" << resource_name << "not found! skipping ...";
                                        continue;
                                }
                                while (resource_in_file.exists()) {
                                        append_file(resource_in_file, resource_out_file);
                                        ++resource_part;
                                        resource_in_file.setFileName(QString(":/%1.%2").arg(resource_name).arg(resource_part));
                                }
                } else {
                        append_file(resource_in_file, resource_out_file);
                }

                resource_out_file.setPermissions(resource_out_file.permissions() | QFile::ExeOwner);
        }






        vate_ui(false);
        open_action->setEnabled(true);

        preview_throttle.break_time_ms = 1000.0; //only show preview images every 1 second while encoding (20130331)
        progress_throttle.break_time_ms = 1000.0; //only update the progress bar every 1 second while encoding (20130331)



        QRect desktop_rect = QApplication::desktop()->screenGeometry();
        move(settings->value("window_location_x", desktop_rect.width()/4).toInt(),
             settings->value("window_location_y", desktop_rect.height()/4).toInt());
        resize(settings->value("window_size_width", desktop_rect.width()/2).toInt(),
               settings->value("window_size_height", desktop_rect.height()/2).toInt());
        statid_line_1->setText(settings->value("statid1").toString());
        statid_line_2->setText(settings->value("statid2").toString());
        statid_line_3->setText(settings->value("statid3").toString());
        statid_group_box->setChecked(settings->value("statid_is_checked").toBool());
        sda_logo_checkbox->setChecked(settings->value("statid_sda_logo_is_checked").toBool());
        audio_commentary_checkbox->setChecked(settings->value("statid_audio_commentary_is_checked").toBool());

        output_path = settings->value("output_path").toString();
        if (output_path.isEmpty() || !QFileInfo(output_path).exists()) {
                output_path = desktop_path_string;
        }
        set_output_path_current_label();

        output_name_edit->setText(settings->value("output_name", "yua").toString());



        //
        //open("/down/test/Piston Hurrican 5.49.mp4");
        //open("/down/test/Clip.avi");
        //open("/down/test/genesis2.vob");
        //open("/down/test/asd.avi");
        //open("/down/test/pso.vob");
        //open("Y:/down/test/prime.vob");
        //open("/home/parallels/Desktop/Parallels Shared Folders/down/test/genesis2.vob");
        //open("y:/down/Heaven_1920_1200_30_LAGSRGB_5sec.avi");

        cli_jobs_to_process = false;
        cli_trim_start_frame = cli_trim_end_frame = 0;
        cli_files_to_append = false;

        QStringList args = QApplication::arguments();
        args.pop_front(); //we don't care about the invocation name (20130328)

        QRegExp help_regex("-h");
        QRegExp help_regex_long("--help");
        QRegExp version_regex("-v");
        QRegExp version_regex_long("--version");

        QRegExp q_regex("--qualities=(.+)");
        QRegExp output_regex("--output=(.+)");
        QRegExp output_basename_regex("--output-basename=(.+)");
#ifdef WITH_NNEDI3
        QRegExp interlaced_regex("--interlaced");
#endif
        QRegExp progressive_regex("--progressive");
        QRegExp f_regex("--f=?(\\d)");
        QRegExp d_regex("--d=?(\\d)");
        QRegExp fdp_2d_regex("--2d");
        QRegExp fdp_3d_regex("--3d");
        QRegExp mono_regex("--mono");
        QRegExp statid_regex("--statid");
        QRegExp statid_line_regex("--statid(\\d)=(.+)");
#ifdef WITH_NNEDI3
        QRegExp bff_regex("--bff");
        QRegExp tff_regex("--tff");
#endif
        QRegExp standard_regex("--standard");
        QRegExp widescreen_regex("--widescreen");
        QRegExp one_pixel_shift_regex("--1-pixel-shift");
        QRegExp alternate_one_pixel_shift_regex("--alternate-1-pixel-shift");
        QRegExp de_deflicker_regex("--de-deflicker");
        QRegExp alternate_de_deflicker_regex("--alternate-de-deflicker");
        QRegExp shut_down_when_done_regex("--shut-?down");
        QRegExp trim_regex("--trim=(\\d+,\\d+)");
        QRegExp input_regex("--input=(.+)");
        QRegExp append_regex("--append=(.+)");
        QRegExp xq_bitrate_regex("--xq-bitrate=(\\d+)");
        bool cli_force_interlaced = false;
        bool cli_force_progressive = false;
        QMap<int, QString> statid_temp;
        bool cli_statid = false;
        bool cli_open_file = false;
        cli_aspect_ratio = false;
        cli_set_qualities_automatically = false;
        QString cli_open_filepath;
        foreach (QString arg, args) {
                if (q_regex.indexIn(arg) > -1) {
                        QString letters = q_regex.cap(1).toLower();
                        QStringList possible_q = QStringList() << "l" << "m" << "h" << "i" << "x";
                        foreach (QString q, possible_q) {
                                if (letters.contains(q)) {
                                        cli_qualities << q;
                                }
                        }

                } else if (output_basename_regex.indexIn(arg) > -1) {
                        output_name_edit->setText(output_basename_regex.cap(1));

		} else if (output_regex.indexIn(arg) > -1) {
                        output_path = output_regex.cap(1);
                        set_output_path_current_label();

#ifdef WITH_NNEDI3
                } else if (interlaced_regex.indexIn(arg) > -1) {
                        cli_force_interlaced = true;
#endif

                } else if (progressive_regex.indexIn(arg) > -1) {
                        cli_force_progressive = true;

                } else if (f_regex.indexIn(arg) > -1) {
                        if (f_regex.cap(1).toInt() == 1) f1_button->setChecked(true);
                        else if (f_regex.cap(1).toInt() == 2) f2_button->setChecked(true);
                        else if (f_regex.cap(1).toInt() == 3) f3_button->setChecked(true);
                        else qDebug() << this << "unknown f" << f_regex.cap(1);

                } else if (d_regex.indexIn(arg) > -1) {
                        if (d_regex.cap(1).toInt() == 1) d1_button->setChecked(true);
                        else if (d_regex.cap(1).toInt() == 4) d4_button->setChecked(true);
                        else qDebug() << this << "unknown d" << d_regex.cap(1);

                } else if (statid_line_regex.indexIn(arg) > -1) {
                        int number = statid_line_regex.cap(1).toInt();
                        if (number == 1 || number == 2 || number == 3) statid_temp[number] = statid_line_regex.cap(2);
                        else qDebug() << this << "unknown statid line number" << number;

                } else if (fdp_2d_regex.indexIn(arg) > -1) {
                        fdp_2d_button->setChecked(true);
                } else if (fdp_3d_regex.indexIn(arg) > -1) {
                        fdp_3d_button->setChecked(true);
                } else if (mono_regex.indexIn(arg) > -1) {
                        mono_checkbox->setChecked(true);
                } else if (standard_regex.indexIn(arg) > -1) {
                        cli_aspect_ratio = true;
                        standard_button->setChecked(true);
                } else if (widescreen_regex.indexIn(arg) > -1) {
                        cli_aspect_ratio = true;
                        widescreen_button->setChecked(true);
                } else if (one_pixel_shift_regex.indexIn(arg) > -1) {
                        one_pixel_bob_button->setChecked(true);
                } else if (alternate_one_pixel_shift_regex.indexIn(arg) > -1) {
                        alternate_one_pixel_bob_button->setChecked(true);
                } else if (de_deflicker_regex.indexIn(arg) > -1) {
                        retard_bob_button->setChecked(true);
                } else if (alternate_de_deflicker_regex.indexIn(arg) > -1) {
                        alternate_retard_bob_button->setChecked(true);
                } else if (statid_regex.indexIn(arg) > -1) {
                        cli_statid = true;
#ifdef WITH_NNEDI3
		} else if (tff_regex.indexIn(arg) > -1) {
                        tff_button->setChecked(true);
                        force_field_dominance = true;
                } else if (bff_regex.indexIn(arg) > -1) {
                        bff_button->setChecked(true);
                        force_field_dominance = true;
#endif
		} else if (trim_regex.indexIn(arg) > -1) {
                        QStringList string_list = trim_regex.cap(1).split(',');
                        if (string_list.size() == 2) {
                                cli_trim_start_frame = string_list.at(0).toInt();
                                cli_trim_end_frame = string_list.at(1).toInt();
                        }
                } else if (input_regex.indexIn(arg) > -1) {
                        cli_jobs_to_process = true;
                        cli_open_file = true;
                        cli_open_filepath = input_regex.cap(1);
                } else if (append_regex.indexIn(arg) > -1) {
                        cli_files_to_append = true;
                        cli_filenames_to_append << append_regex.cap(1);
                } else if (shut_down_when_done_regex.indexIn(arg) > -1) {
                        shut_down_when_done_checkbox->setChecked(true);
                } else if (xq_bitrate_regex.indexIn(arg) > -1) {
                        xq_bitrate_kbit = xq_bitrate_regex.cap(1).toInt();
                } else if (help_regex.indexIn(arg) > -1 || help_regex_long.indexIn(arg) > -1) {
                        displayHelp();
                } else if (version_regex.indexIn(arg) > -1 || version_regex_long.indexIn(arg) > -1) {
                        displayVersion();
                } else {
                        qDebug() << this << "unknown command line option" << arg;
                }
        }

        statid_group_box->setChecked(false);
        if (cli_statid) {
                statid_group_box->setChecked(true);
                statid_line_1->setText(statid_temp[1]);
                statid_line_2->setText(statid_temp[2]);
                statid_line_3->setText(statid_temp[3]);
        }

        emit set_cli_force(cli_force_interlaced, cli_force_progressive);

        if (cli_open_file) {
                if (!cli_qualities.size()) {
                        cli_set_qualities_automatically = true;
                }
                open(cli_open_filepath);
        }
}

Yua::~Yua() {
        decoder_thread.quit();
        decoder_thread.wait();

        deinterlacer_thread.quit();
        deinterlacer_thread.wait();
        deinterlacer2_thread.quit();
        deinterlacer2_thread.wait();

        audio_encoder_thread.quit();
        audio_encoder_thread.wait();

        video_encoder_thread.quit();
        video_encoder_thread.wait();

        foreach (QString resource_outpath, resource_outpaths) {
                QFile::remove(resource_outpath);
        }
        QDir().rmdir(temp_dir);
}

void Yua::save_settings_before_exiting() {
        settings->setValue("window_location_x", pos().x());
        settings->setValue("window_location_y", pos().y());
        settings->setValue("window_size_width", width());
        settings->setValue("window_size_height", height());

        settings->setValue("statid1", statid_line_1->text());
        settings->setValue("statid2", statid_line_2->text());
        settings->setValue("statid3", statid_line_3->text());
        settings->setValue("statid_is_checked", statid_group_box->isChecked());
        settings->setValue("statid_sda_logo_is_checked", sda_logo_checkbox->isChecked());
        settings->setValue("statid_audio_commentary_is_checked", audio_commentary_checkbox->isChecked());

        settings->setValue("output_name", output_name_edit->text());
        settings->setValue("output_path", output_path);

        settings->sync(); //why is this necessary? (20130207)
}

void Yua::exit_yua() {
        if (currently_encoding) {
                if (QMessageBox::information(this,
                                             tr("Exit Yua"),
                                             tr("An encode is currently in progress. Really exit Yua?"),
                                             QMessageBox::Ok | QMessageBox::Cancel
                                             ) == QMessageBox::Cancel) {
                        return;
                }
        }

        save_settings_before_exiting();
        QApplication::quit();
}


void Yua::closeEvent(QCloseEvent *event) {
        if (currently_encoding) {
                if (QMessageBox::information(this,
                                             tr("Exit Yua"),
                                             tr("An encode is currently in progress. Really exit Yua?"),
                                             QMessageBox::Ok | QMessageBox::Cancel
                                             ) == QMessageBox::Cancel) {
                        event->ignore();
                        return;
                }
        }

        save_settings_before_exiting();
        event->accept();
}


void Yua::about() {
        QMessageBox::about(this, tr("About Yua"),

                           tr("<i>\"it's about the strength of my desire\"</i>"

                              "<br><br>\n<a href=\"http://forum.speeddemosarchive.com/post/yua.html\">Yua</a> version %1 copyright %2 Taiga Software LLC"
                              "<br><br>\nYour use of this software is governed by the terms of the <a href=\"http://www.gnu.org/licenses/gpl.html\">GPL</a> version 2 or, at your option, any later version."
#ifdef STATIC_YUA
                              "<br><br>\nThis software uses libraries and binaries from the <a href=\"http://ffmpeg.org/\">FFmpeg project</a> and the <a href=\"http://x264.nl/\">x264 library</a>, released "
                                        "under the <a href=\"http://www.gnu.org/licenses/gpl.html\">GPL</a>, the <a href=\"http://opencore-amr.sourceforge.net/\">FDK AAC library</a>, released under the "
                                        "under the <a href=\"http://sourceforge.net/p/opencore-amr/fdk-aac/ci/master/tree/NOTICE\">Fraunhofer-FDK-AAC-for-Android license</a>, and the MP4Box binary from the "
                                        "<a href=\"http://gpac.wp.mines-telecom.fr/\">GPAC Project on Advanced Content</a>, released under the <a href=\"http://www.gnu.org/licenses/lgpl.html\">LGPL</a>."
#endif
                              "<br><br>\nAviSynth timebase reduction code by RAYMOD2 and IanB"
                              "<br><br>\nAviSynth 3.0 high quality yv12 deinterlacing code by <a href=\"http://forum.doom9.org/showthread.php?p=1295182#post1295182\">Manao</a>"
                              ).arg(YUA_VERSION).arg(QDate::currentDate().year()));
}

QRect Yua::calculate_new_cropping_rect(QSize new_size) {
        double x_ratio = (double)new_size.width() / cropping_rect_set_from_size.width();
        double y_ratio = (double)new_size.height() / cropping_rect_set_from_size.height();
        QRect new_cropping_rect = cropping_rect;

        new_cropping_rect.setLeft(x_ratio * cropping_rect.left());
        new_cropping_rect.setRight(x_ratio * cropping_rect.right());
        new_cropping_rect.setTop(y_ratio * cropping_rect.top());
        new_cropping_rect.setBottom(y_ratio * cropping_rect.bottom());

        new_cropping_rect.setWidth(new_cropping_rect.width() - new_cropping_rect.width()%2);
        new_cropping_rect.setHeight(new_cropping_rect.height() - new_cropping_rect.height()%2);

        return new_cropping_rect;
}

void Yua::queue_frame_slot(Frame frame) {
        //qDebug() << "queue_frame_slot(): frame id" << frame.pts << "size" << frame.image.size();
        frameid_frame.insert(frame.pts, frame);
        while (!frameid_queue.isEmpty()) {
                qint64 next_frameid = frameid_queue.head();
                if (!frameid_frame.contains(next_frameid)) {
                        //qDebug() << "queue_frame_slot(): could not find frameid" << next_frameid << "in frameid_frame";
                        break;
                }
                frameid_queue.removeFirst();
                Frame frame = frameid_frame.take(next_frameid);

                if (crop_checkbox->isChecked() && !frame.statid) {
                        QRect new_cropping_rect = calculate_new_cropping_rect(frame.image.size());
                        QImage new_image(new_cropping_rect.size(), frame.image.format());
                        QPainter painter(&new_image);
                        painter.drawImage(new_image.rect(), frame.image, new_cropping_rect);
                        painter.end();
                        frame.image = new_image;
                }

                //qDebug() << "queue_frame_slot(): encoding frame id" << frame.pts;
                emit encode_frame(frame);
                if (!preview_throttle.busy()) {
                        emit encode_frame_preview(frame.image);
                        preview_throttle.done();
                }
        }
        //qDebug() << "queue_frame_slot(): stopping: frameid_frame contains" << frameid_frame.size() << "frames, frameid_queue.size() is" << frameid_queue.size();
}

void Yua::d4_filter_display_clicked() {
        if (currently_encoding) return;
        QObject *sender = qobject_cast<QObject *>(QObject::sender());

        if (&no_change_display == sender) {
                no_change_button->setChecked(true);
        } else if (&one_pixel_bob_display == sender) {
                one_pixel_bob_button->setChecked(true);
        } else if (&alternate_one_pixel_bob_display == sender) {
                alternate_one_pixel_bob_button->setChecked(true);
        } else if (&retard_bob_display == sender) {
                retard_bob_button->setChecked(true);
        } else if (&alternate_retard_bob_display == sender) {
                alternate_retard_bob_button->setChecked(true);
        }

        return produce_filtered_images();
}

void Yua::append_file(QFile &resource_in_file, QFile &resource_out_file) {
        resource_in_file.open(QIODevice::ReadOnly);
        QByteArray resource_data = resource_in_file.readAll();
        resource_in_file.close();

        resource_out_file.open(QIODevice::WriteOnly | QIODevice::Append);
        resource_out_file.write(resource_data);
        resource_out_file.close();
}


void Yua::dragEnterEvent(QDragEnterEvent *event) {
        event->acceptProposedAction();
}
void Yua::dragMoveEvent(QDragMoveEvent *event) {
        event->acceptProposedAction();
}
void Yua::dragLeaveEvent(QDragLeaveEvent *event) {
        event->accept();
}
void Yua::dropEvent(QDropEvent *event) {
        if (currently_encoding) {
                return event->acceptProposedAction();
        }

        const QMimeData *mime_data = event->mimeData();
        if (mime_data->hasUrls()) {
                QList<QUrl> urls = mime_data->urls();
                if (urls.size() != 1) {
                        QMessageBox::critical(this, tr("Error"), tr("Sorry, I only support opening one source video at a time right now."));
                        goto finish;
                }

                foreach (QUrl filename, urls) {
                        open(filename.toLocalFile());
                }
        }

        finish:
        event->acceptProposedAction();
}


void Yua::set_slider_max() {
        int framecount = video_info.number_of_frames;
        if (video_info.interlaced) framecount *= 2;
        slider.setRange(0, framecount-1);
        qDebug() << "setting slider to range 0," << framecount-1 << "; interlaced is" << video_info.interlaced;
        //emit set_estimated_total_frames_to_encode(framecount);
        if (slider_resume_value != -9999) {
                int old_slider_value = slider.value();
                slider.setValue(slider_resume_value);
                if (old_slider_value == slider_resume_value) { //otherwise the slot will not be triggered (20130208)
                        slider_moved(slider.value());
                }
                slider_resume_value = -9999;
        }
}

/*
void Yua::set_framerate(double new_framerate) {
        framerate = new_framerate;
        statid_length_seconds = 5.0;
        statid_length_frames = int(framerate * statid_length_seconds);
}
*/

void Yua::index_begun() {
        set_status(tr("Yua is examining the video ... "));
}

void Yua::index_finished() {
        set_status(tr("done.\n"));
        progress_bar.setValue(0);
}

void Yua::normalize_begun(QString filename) {
        set_status(tr("Determining the audio amplification factor of \"%1\" ... ").arg(QFileInfo(filename).fileName()));
}

void Yua::normalize_finished(QString filename) {
        Q_UNUSED(filename)
        set_status(tr("done.\n"));
}

void Yua::index_progress(qint64 frames_read, qint64 estimated_total_frames) {
        if (!estimated_total_frames) {
                static int fake_progress = 0;
                fake_progress += progress_bar.maximum() / 10;
                if (fake_progress > progress_bar.maximum()) fake_progress = 0;
                progress_bar.setValue(fake_progress);
                return;
        }

        progress_bar.setValue(((double)frames_read / (double)estimated_total_frames) * (double)progress_bar.maximum());
}

void Yua::interlaced_changed() {
        slider_resume_value = interlaced_button->isChecked() ? slider.value()*2 : slider.value()/2;
        emit force_interlaced(interlaced_button->isChecked());
}

int Yua::round_up_to_mod_4(int number) {
        while (number % 4) ++number;
        return number;
}

int Yua::round_up_to_mod_2(int number) {
        while (number % 2) ++number;
        return number;
}

void Yua::set_width_from_height_clamped_to_native_width_and_mod_2_height(int &width, int &height) {
        width = round_up_to_mod_4(height / aspect_ratio);
        if (width > video_info.width) { //oops - don't make it wider than native width - x264's "sar" functionality will take care of displaying it wider (20130212)
                width = round_up_to_mod_4(video_info.width);
        }
        height = round_up_to_mod_2(height);
}

void Yua::set_sizes() {
        native_width = video_info.width;
        native_height = video_info.height;
        aspect_ratio_num = native_width;
        aspect_ratio_den = native_height;

        if (aspect_ratio_group_box->isChecked()) {
                aspect_ratio_num = widescreen_button->isChecked() ? 16 : 4;
                aspect_ratio_den = widescreen_button->isChecked() ?  9 : 3;
                if (d4_button->isChecked()) {
                        native_height /= 2;
                }
        } else if (d4_button->isChecked()) { //if the aspect ratio checkbox is not checked and the d1 box is checked we do nothing - if d4 is checked we reduce to d4 (20130212)
                native_height = 240;
        }
        aspect_ratio = (double)aspect_ratio_den / (double)aspect_ratio_num;
        emit set_aspect_ratio(aspect_ratio_num, aspect_ratio_den); //we will use this to calculate the pixel aspect ratio passed to x264 so that we can display at the correct aspect ratio without stretching the image horizontally and without losing resolution vertically (i.e., without setting the height from the width) (20130213)


        //heights
        lq_height = 240;
        if (lq_height > native_height) lq_height = native_height;

        mq_height = 240;
        if (mq_height > native_height) mq_height = native_height;

        hq_height = 480;
        if (hq_height > native_height) hq_height = native_height;

        iq_height = 768;
        if (iq_height > native_height) iq_height = native_height;

        xq_height = native_height;

        //widths
        set_width_from_height_clamped_to_native_width_and_mod_2_height(native_width, native_height);
        set_width_from_height_clamped_to_native_width_and_mod_2_height(lq_width, lq_height);
        set_width_from_height_clamped_to_native_width_and_mod_2_height(mq_width, mq_height);
        set_width_from_height_clamped_to_native_width_and_mod_2_height(hq_width, hq_height);
        set_width_from_height_clamped_to_native_width_and_mod_2_height(iq_width, iq_height);
        set_width_from_height_clamped_to_native_width_and_mod_2_height(xq_width, xq_height);


//        qDebug() << "aspect ratio" << aspect_ratio;
//        qDebug() << "native" << native_width << native_height;
//        qDebug() << "lq" << lq_width << lq_height;
//        qDebug() << "mq" << mq_width << mq_height;
//        qDebug() << "hq" << hq_width << hq_height;
//        qDebug() << "iq" << iq_width << iq_height;
//        qDebug() << "xq" << xq_width << xq_height;


        QStringList recommended_qualities = set_recommended_qualities();
        if (cli_set_qualities_automatically) {
                cli_qualities = recommended_qualities;
        }
        set_quality_checkboxes(cli_qualities);


        preview_size = QSize(native_width, native_height);
        emit set_preview_size(preview_size.width(), preview_size.height());
        cropping_rect = calculate_new_cropping_rect(preview_size);
        cropping_rect_set_from_size = preview_size;
        emit_set_statid_size();
        return process_current_image();
}

void Yua::emit_set_statid_size() {
        //emit set_statid_size(preview_size.width(), preview_size.height());
        //
        int sar_num = aspect_ratio_num * video_info.height;
        int sar_den = aspect_ratio_den * video_info.width;
        double sar = (double)sar_num / sar_den;
        qDebug() << "set_statid_size(" << native_width << "*" << sar << ", " << preview_size.height() << ")";
        emit set_statid_size(native_width * sar, preview_size.height());
        //
        if (crop_checkbox->isChecked()) emit set_statid_size(cropping_rect.width(), cropping_rect.height());
}

void Yua::seek_to(int seek_to_frame) {
        seek_jobs.enqueue(seek_to_frame);
        emit seek();
}

void Yua::slider_moved(int slider_pos) {
        //qDebug() << this << "slider_moved:" << slider_pos;
        if (currently_encoding) {
                qDebug() << this << "slider_moved: currently_encoding, so canceled seek";
                return;
        }
        int seek_to_frame = slider_pos;

//        if (statid_group_box->isChecked()) {
//                if (slider_currently_in_statid()) {
//                        QImage statid_image = make_statid_image();
//                        emit preview_ready(statid_image);
//                        emit preview_ready_d4(statid_image);
//                        return;
//                }

//                seek_to_frame -= statid_length_frames;
//        }

        if (interlaced_button->isChecked()) seek_to_frame /= 2;

        return seek_to(seek_to_frame);
}

void Yua::restart_decoder() {
        if (encoding_audio) {
                set_status(tr("Encoding %1 video (second pass) and audio ...\n").arg(current_job.name));
        } else {
                set_status(tr("Encoding %1 video (second pass) ...\n").arg(current_job.name));
        }

        int start = trim_start_frame;
        int stop = trim_end_frame;
        if (video_info.interlaced) {
                start = (start+1) / 2;
                stop = (stop+1) / 2;
        }
        emit decoding_start(current_job.id+1, start, stop);
}

void Yua::open_dialog() {
        QString filename = QFileDialog::getOpenFileName(this, tr("Open Video File"), settings->value("last_file_location", desktop_path_string).toString());
        if (!filename.isNull()) {
                settings->setValue("last_file_location", QFileInfo(filename).absolutePath());
                return open(filename);
        }
}
void Yua::append_dialog() {
        QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Append Video File"), settings->value("last_file_location", desktop_path_string).toString());
        if (!filenames.isEmpty()) {
                settings->setValue("last_file_location", QFileInfo(filenames.at(0)).absolutePath());
                return append(filenames);
        }
}

void Yua::open(QString filename) {
        qDebug() << "open" << filename;
        if (filename.isNull()) return;
        vate_ui(false);
        emit open_file(filename);
}
void Yua::append(QStringList filenames) {
        qDebug() << "append()" << filenames;
        if (filenames.isEmpty()) return;
        vate_ui(false);
        emit append_files(filenames);
}

void Yua::open_successful(QStringList filenames, Video_Information new_video_info, bool new_has_audio, Audio_Information new_audio_info) {
        qDebug() << "open_successful()";
        has_audio = new_has_audio;
        video_info = new_video_info;
        audio_info = new_audio_info;

        emit set_audio_source_information(audio_info);
        emit set_video_timebase(video_info.timebase_num, video_info.timebase_den);
        if (!force_field_dominance) tff_button->setChecked(video_info.tff);
        interlaced_button->setChecked(video_info.interlaced);
        progressive_button->setChecked(!video_info.interlaced);

        //from interlaced_changed()
        field_dominance_group_box->setVisible(interlaced_button->isChecked());
        aspect_ratio_group_box->setChecked(cli_aspect_ratio || interlaced_button->isChecked());

        d_changed();
        set_slider_max();
        set_sizes();
        //

        if (!filenames.isEmpty()) {
                setWindowTitle(tr("Yua - %1").arg(filenames.join(", ")));
        }

        trim_start_frame = trim_end_frame = 0;
        if (cli_jobs_to_process) {
                trim_start_frame = cli_trim_start_frame;
                trim_end_frame = cli_trim_end_frame;
        }
        make_trimpoints_label();

        if (statid_group_box->isChecked()) {
                statid_changed();
        }

        if (cli_jobs_to_process) {
//                if (cli_files_to_append) {
//                        return;
//                }
//                qDebug() << "encoding_start() in 1 second";
//                QTimer::singleShot(1000, this, SLOT(encoding_start())); //give the ui time to finish firing all its signals from its initial setup and the effects to take effect - e.g. force_interlaced() (20130327)
                return;
        }

        qDebug() << this << "video_info.framerate is" << video_info.framerate;

        slider.setValue(0);
        return slider_moved(0); //the slot may not fire on its own (20130602)
}

QImage Yua::make_selected_image(Frame in_frame) {
        if (in_frame.isNull()) {
                qDebug() << "make_selected_image() null in_frame!";
                return QImage();
        }

        QImage out_image = in_frame.image.copy();
        QPainter painter(&out_image);
        painter.setBrush(QBrush(Qt::transparent));
        QPen pen(QColor(0x00, 0x9b, 0x58), 8);
        painter.setPen(pen);
        painter.drawRect(out_image.rect());
        painter.end();
        return out_image;
}

QImage Yua::add_d4_caption_to_image(QString text, Frame in_frame) {
        if (in_frame.isNull()) {
                qDebug() << "add_d4_caption_to_image() null in_frame!";
                return QImage();
        }

        QImage out_image = in_frame.image.copy();
        QPainter painter(&out_image);
        QPen pen(Qt::white);
        painter.setPen(pen);
        painter.setFont(QFont("Verdana"));
        painter.drawText(out_image.rect(), Qt::AlignHCenter, text);
        painter.end();
        return out_image;
}

QImage Yua::draw_frame_number_and_timestamp_on_image(Frame in_frame) {
        if (in_frame.isNull()) {
                qDebug() << "add_frame_number_and_timestamp_to_image() null in_frame!";
                return QImage();
        }

        int frame_number = slider.value();
        QImage out_image = in_frame.image.copy();
        QPainter painter(&out_image);
        QPen pen(Qt::white);
        painter.setPen(pen);
        painter.setFont(QFont("Verdana"));
        painter.drawText(out_image.rect().adjusted(10, 0, -10, -10), Qt::AlignLeft | Qt::AlignBottom, QString("%1").arg(frame_number));
        display_timestamp_datetime.setMSecsSinceEpoch(frame_number * (1/video_info.framerate*1000));
        painter.drawText(out_image.rect().adjusted(0, 0, -10, -10), Qt::AlignRight | Qt::AlignBottom, display_timestamp_datetime.toString("hh:mm:ss.zzz"));
        painter.end();
        return out_image;
}

QImage Yua::draw_cropping_rect_on_image(QImage in_image) {
        QImage bg(in_image);
        bg.fill(Qt::red);
        QPainter painter(&bg);
        painter.drawImage(cropping_rect, in_image, cropping_rect);
        QRect text_rect = cropping_rect;
        if (in_image.height() - cropping_rect.bottom() > 28) {
                text_rect.setBottom(text_rect.bottom() + 24);
        } else {
                text_rect.setBottom(text_rect.bottom() - 10);
        }
        QPen pen(Qt::white);
        painter.setPen(pen);
        painter.drawText(text_rect, Qt::AlignBottom | Qt::AlignHCenter, QString("%1 x %2").arg(cropping_rect.width()).arg(cropping_rect.height()));
        painter.end();
        return bg;
}

void Yua::draw_preview() {
        QImage bg = draw_cropping_rect_on_image(current_preview_image.image);

        Frame preview_image(current_preview_image);
        if (crop_checkbox->isChecked() && !current_preview_image.statid) preview_image.image = bg;

        screenshot_image = preview_image.image.copy();
        preview_image = draw_frame_number_and_timestamp_on_image(preview_image);
        emit preview_ready(preview_image.image);
}

void Yua::handle_d4_cropping_overlay(Frame &frame) {
        if (!crop_checkbox->isChecked()) return;
        frame.image = draw_cropping_rect_on_image(frame.image);
}

void Yua::new_preview(Frame preview_image) {
        if (currently_encoding || !frameid_queue.isEmpty()) { //sometimes muxing happens so fast that we receive frames from the deinterlacer after we have stopped currently_encoding - this frame will be "orphaned" across the entire toolchain (bad) unless we go ahead and handle it here (20130605)
                emit queue_video_frame(preview_image);
//                if (!preview_throttle.busy()) {
//                        emit preview_ready(preview_image.image);
//                        preview_throttle.done();
//                }
        } else {
                //qDebug() << this << "received preview image" << preview_image.pts << "while not currently_encoding";
                current_preview_image = preview_image;
                draw_preview();
        }
}

void Yua::new_image(Frame frame) {
        if (currently_encoding) {
                //qDebug() << this << "receive frame id" << frame.pts << frame.second_field_pts << "for job id" << frame.job_id << "current job id" << current_job.id;
                frameid_queue << frame.pts;
                if (interlaced_button->isChecked()) {
                        frameid_queue << frame.second_field_pts;
                }
        }// else qDebug() << this << "!currently_encoding: receive frame id" << frame.pts << frame.second_field_pts;

        current_image = frame;
        return process_current_image();
}

void Yua::process_current_image() {
        if (interlaced_button->isChecked()) {
                if (current_image.statid) {
                        //qDebug() << "preview_ready_d4" << current_image.image.size();
                        emit preview_ready_d4(current_image.image);
                        emit preview_ready(current_image.image);
                        if (currently_encoding) {
                                emit queue_video_frame(current_image);
                                Frame second_field(current_image);
                                second_field.pts = current_image.second_field_pts;
                                emit queue_video_frame(second_field);
                        }
                } else {
                        emit deinterlace(current_image, d4_button->isChecked());
                }
        } else {
                emit make_preview(current_image);
        }
        //emit input_image_ready(current_image);
}

bool Yua::is_even_field() {
        int slider_value = slider.value();
        bool even_field = (slider_value % 2 == 0);
        if (!tff_button->isChecked()) even_field = !even_field;
        return even_field;
}

void Yua::top_bottom_field(Frame new_top_field_image, Frame new_bottom_field_image) {
        if (currently_encoding && !tff_button->isChecked()) {
                qSwap(new_top_field_image.pts, new_bottom_field_image.pts);
        }

        //these globals are for redrawing the output preview if the user changes the field dominance - not for encoding because they will be invalid long before the encoder reaches them (20130121)
        top_field_image = new_top_field_image;
        bottom_field_image = new_bottom_field_image;

        if (d1_button->isChecked()) {
                if (currently_encoding) {
                        if (tff_button->isChecked()) {
                                emit double_height(new_top_field_image, true);
                                emit double_height(new_bottom_field_image, false);
                        } else {
                                emit double_height(new_bottom_field_image, false);
                                emit double_height(new_top_field_image, true);
                        }
                } else {
                        if (is_even_field()) {
                                emit double_height(new_top_field_image, true);
                        } else {
                                emit double_height(new_bottom_field_image, false);
                        }
                }

                return;
        }

        //d4
        if (currently_encoding && no_change_button->isChecked()) {
                if (tff_button->isChecked()) {
                        emit queue_video_frame(new_top_field_image);
                        emit queue_video_frame(new_bottom_field_image);
                } else {
                        emit queue_video_frame(new_bottom_field_image);
                        emit queue_video_frame(new_top_field_image);
                }
        }
        return produce_filtered_images();
}

void Yua::height_doubled(Frame height_doubled_image) {
        return new_preview(height_doubled_image);
}

void Yua::produce_filtered_images() {
        if (top_field_image.isNull() || bottom_field_image.isNull()) return;

        if (!currently_encoding || no_change_button->isChecked()) {
                Frame frame = is_even_field() ? top_field_image : bottom_field_image;
                if (!currently_encoding) {
                        handle_d4_cropping_overlay(frame);
                        screenshot_image = frame.image.copy();
                        frame = add_d4_caption_to_image(tr("no change"), frame);
                        if (no_change_button->isChecked()) {
                                frame = draw_frame_number_and_timestamp_on_image(frame);
                                frame = make_selected_image(frame);
                        }
                }
                if (!currently_encoding) {
                        emit no_change_image_ready(frame.image);
                        //preview_throttle.done();
                }
        }

        if (!currently_encoding || one_pixel_bob_button->isChecked() || alternate_one_pixel_bob_button->isChecked()) {
                emit push_down_one_line(top_field_image, bottom_field_image);
        }
        if (!currently_encoding || retard_bob_button->isChecked() || alternate_retard_bob_button->isChecked()) {
                emit retard_bob(top_field_image, bottom_field_image, false);
                emit retard_bob(top_field_image, bottom_field_image, true);
        }
}

void Yua::one_pixel_bobbed(Frame top_field_image, Frame bottom_field_image, Frame top_field_image_pushed_down, Frame bottom_field_image_pushed_down) {
        if (currently_encoding) {
                if (one_pixel_bob_button->isChecked()) {
                        if (tff_button->isChecked()) {
                                emit queue_video_frame(top_field_image);
                                emit queue_video_frame(bottom_field_image_pushed_down);
                        } else {
                                emit queue_video_frame(bottom_field_image_pushed_down);
                                emit queue_video_frame(top_field_image);
                        }
//                        if (!preview_throttle.busy()) {
//                                emit one_pixel_bob_image_ready(top_field_image.image);
//                                preview_throttle.done();
//                        }
                } else if (alternate_one_pixel_bob_button->isChecked()) {
                        if (tff_button->isChecked()) {
                                emit queue_video_frame(top_field_image_pushed_down);
                                emit queue_video_frame(bottom_field_image);
                        } else {
                                emit queue_video_frame(bottom_field_image);
                                emit queue_video_frame(top_field_image_pushed_down);
                        }
//                        if (!preview_throttle.busy()) {
//                                emit alternate_one_pixel_bob_image_ready(top_field_image_pushed_down.image);
//                                preview_throttle.done();
//                        }
                }
        } else {
                Frame frame_1 = is_even_field() ? top_field_image : bottom_field_image_pushed_down;
                Frame frame_2 = is_even_field() ? top_field_image_pushed_down : bottom_field_image;

                handle_d4_cropping_overlay(frame_1);
                frame_1 = add_d4_caption_to_image(tr("1-pixel shift"), frame_1);
                if (one_pixel_bob_button->isChecked()) {
                        frame_1 = draw_frame_number_and_timestamp_on_image(frame_1);
                        frame_1 = make_selected_image(frame_1);
                }
                emit one_pixel_bob_image_ready(frame_1.image);

                handle_d4_cropping_overlay(frame_2);
                frame_2 = add_d4_caption_to_image(tr("alternate 1-pixel shift"), frame_2);
                if (alternate_one_pixel_bob_button->isChecked()) {
                        frame_2 = draw_frame_number_and_timestamp_on_image(frame_2);
                        frame_2 = make_selected_image(frame_2);
                }
                emit alternate_one_pixel_bob_image_ready(frame_2.image);
        }
}

void Yua::retard_bobbed(Frame top_field_image_retard_bobbed, Frame bottom_field_image_retard_bobbed, bool alternate) {
        if (currently_encoding) {
                if ((retard_bob_button->isChecked() && !alternate) || (alternate_retard_bob_button->isChecked() && alternate)) {
                        if (tff_button->isChecked()) {
                                emit queue_video_frame(top_field_image_retard_bobbed);
                                emit queue_video_frame(bottom_field_image_retard_bobbed);
                        } else {
                                emit queue_video_frame(bottom_field_image_retard_bobbed);
                                emit queue_video_frame(top_field_image_retard_bobbed);
                        }
//                        if (!preview_throttle.busy()) {
//                                if (!alternate) emit retard_bob_image_ready(top_field_image_retard_bobbed.image);
//                                else emit alternate_retard_bob_image_ready(top_field_image_retard_bobbed.image);
//                                preview_throttle.done();
//                        }
                }
        } else {
                Frame image_to_label = is_even_field() ? top_field_image_retard_bobbed : bottom_field_image_retard_bobbed;
                handle_d4_cropping_overlay(image_to_label);
                if (alternate) {
                        Frame frame = add_d4_caption_to_image(tr("alternate de-deflicker"), image_to_label);
                        if (alternate_retard_bob_button->isChecked()) {
                                frame = draw_frame_number_and_timestamp_on_image(frame);
                                frame = make_selected_image(frame);
                        }
                        emit alternate_retard_bob_image_ready(frame.image);
                } else {
                        Frame frame = add_d4_caption_to_image(tr("de-deflicker"), image_to_label);
                        if (retard_bob_button->isChecked()) {
                                frame = draw_frame_number_and_timestamp_on_image(frame);
                                frame = make_selected_image(frame);
                        }
                        emit retard_bob_image_ready(frame.image);
                }
        }
}

void Yua::field_dominance_changed() {
        return produce_filtered_images();
}

void Yua::aspect_ratio_changed() {
        return set_sizes();
}

void Yua::d_changed() {
        preview_display.setVisible(!interlaced_button->isChecked() || d1_button->isChecked());
        no_change_display.setVisible(interlaced_button->isChecked() && d4_button->isChecked());
        d4_options_container_widget->setVisible(interlaced_button->isChecked() && d4_button->isChecked());

        return set_sizes();
}

void Yua::set_output_path_current_label() {
        output_path_current_label->setText(QFileInfo(output_path).fileName());
}

void Yua::output_directory_choose() {
        QString old_location = output_path;
        if (old_location.isEmpty() || !QFileInfo(old_location).exists()) {
                old_location = desktop_path_string;
        }
        output_path = QFileDialog::getExistingDirectory(this, tr("Choose output folder"), old_location);
        if (output_path.isNull()) { //they canceled the dialog (20130122)
                return;
        }
        set_output_path_current_label();
}

QSize Yua::scale_size_cropped(QSize size) {
        QRect new_cropping_rect = calculate_new_cropping_rect(size);
        qDebug() << "new_cropping_rect" << new_cropping_rect;
        return new_cropping_rect.size();
}

void Yua::encoding_start() {
        if (currently_encoding) return;

        if (!lq_button->isChecked()
                        && !mq_button->isChecked()
                        && !hq_button->isChecked()
                        && !iq_button->isChecked()
                        && !xq_button->isChecked()
                        ) {
                set_quality_checkboxes(set_recommended_qualities());
        }

        if (output_path.isEmpty() || !QFileInfo(output_path).exists()) {
                output_directory_choose();
                if (output_path.isEmpty() || !QFileInfo(output_path).exists()) { //they canceled the dialog or something (20130207)
                        return;
                }
        }

        vate_ui(false);
        encoding_abort = false;
        currently_encoding = true;
        power_management.prevent_sleep();

        if (output_name_edit->text().isEmpty()) {
                output_name_edit->setText("yua");
        }

        int f = 1;
        if (f2_button->isChecked()) f = 2;
        else if (f3_button->isChecked()) f = 3;

        int full_f, hq_f, mq_f, lq_f;
        full_f = hq_f = mq_f = lq_f = f;

        qDebug() << this << "framerate is" << video_info.framerate;
        if (f == 1 && (interlaced_button->isChecked() || video_info.framerate > 31)) { //need to apply the requested framerate decimation paradigm (20130113)
                if (fdp_2d_button->isChecked()) {
                        mq_f = 1;
                        lq_f = 3;
                } else { //3d
                        mq_f = lq_f = 2;
                }
        }
        while (video_info.framerate / hq_f > 62) ++hq_f;
        if (fdp_2d_button->isChecked()) {
                while (video_info.framerate / mq_f > 62) ++mq_f;
        } else {
                while (video_info.framerate / mq_f > 31) ++mq_f;
        }
        while (video_info.framerate / lq_f > 31) ++lq_f;
        qDebug() << this << "full_f:" << full_f << " hq_f:" << hq_f << " mq_f:" << mq_f << " lq_f:" << lq_f;

        int default_number_of_channels = 0;
        int low_fi_number_of_channels = 2;
        if (mono_checkbox->isChecked()) {
                default_number_of_channels = low_fi_number_of_channels = 1;
        }

        AVPixelFormat low_fi_colorspace = AV_PIX_FMT_YUV420P;
        AVPixelFormat high_fi_colorspace = low_fi_colorspace;
        if (AV_PIX_FMT_YUV420P != video_info.colorspace) {
                if (AV_PIX_FMT_YUV422P == video_info.colorspace) {
                        high_fi_colorspace = AV_PIX_FMT_YUV422P;
                } else { //probably rgb input
                        high_fi_colorspace = AV_PIX_FMT_YUV444P;
                }
        } else if (interlaced_button->isChecked() && d4_button->isChecked()) { //probably old console material - this will make it look better when viewed full screen (20130602)
                high_fi_colorspace = AV_PIX_FMT_YUV444P;
        }

        QSize xq_size_after_cropping(xq_width, xq_height);
        QSize iq_size_after_cropping(iq_width, iq_height);
        QSize hq_size_after_cropping(hq_width, hq_height);
        QSize mq_size_after_cropping(mq_width, mq_height);
        QSize lq_size_after_cropping(lq_width, lq_height);
        if (crop_checkbox->isChecked()) {
                xq_size_after_cropping = scale_size_cropped(xq_size_after_cropping);
                iq_size_after_cropping = scale_size_cropped(iq_size_after_cropping);
                hq_size_after_cropping = scale_size_cropped(hq_size_after_cropping);
                mq_size_after_cropping = scale_size_cropped(mq_size_after_cropping);
                lq_size_after_cropping = scale_size_cropped(lq_size_after_cropping);
        }

        qDebug() << "xq size_after_cropping" << xq_size_after_cropping;
        qDebug() << "iq size_after_cropping" << iq_size_after_cropping;
        qDebug() << "hq size_after_cropping" << hq_size_after_cropping;
        qDebug() << "mq size_after_cropping" << mq_size_after_cropping;
        qDebug() << "lq size_after_cropping" << lq_size_after_cropping;

        if (xq_button->isChecked()) jobs << Job("XQ"
                                                ,Video_Information(xq_bitrate_kbit, video_info.framerate, full_f, xq_width, xq_height, xq_size_after_cropping.width(), xq_size_after_cropping.height(), high_fi_colorspace)
                                                ,Audio_Information(320000, default_number_of_channels));

        if (iq_button->isChecked()) jobs << Job("IQ"
                                                ,Video_Information(5000, video_info.framerate, full_f, iq_width, iq_height, iq_size_after_cropping.width(), iq_size_after_cropping.height(), high_fi_colorspace)
                                                ,Audio_Information(320000, default_number_of_channels));

        if (hq_button->isChecked()) jobs << Job("HQ"
                                                ,Video_Information(2048, video_info.framerate, hq_f, hq_width, hq_height, hq_size_after_cropping.width(), hq_size_after_cropping.height(), low_fi_colorspace)
                                                ,Audio_Information(128000, default_number_of_channels));

        if (mq_button->isChecked()) jobs << Job("MQ"
                                                ,Video_Information(512, video_info.framerate, mq_f, mq_width, mq_height, mq_size_after_cropping.width(), mq_size_after_cropping.height(), low_fi_colorspace)
                                                ,Audio_Information(64000, low_fi_number_of_channels));

        if (lq_button->isChecked()) jobs << Job("LQ"
                                                ,Video_Information(128, video_info.framerate, lq_f, lq_width, lq_height, lq_size_after_cropping.width(), lq_size_after_cropping.height(), low_fi_colorspace)
                                                ,Audio_Information(64000, low_fi_number_of_channels));

        qDebug() << "encoding_start(): jobs.size() is" << jobs.size();
        return encode_jobs();
}

void Yua::encode_jobs() {
        if (!currently_encoding) return;

        progress_bar.setValue(0);

        if (!jobs.size()) {
                power_management.unprevent_sleep();

                if (cli_jobs_to_process) {
                        qDebug() << "all jobs finished; exiting ...";
                        if (shut_down_when_done_checkbox->isChecked()) {
                                return Power_Management::shut_down();
                        } else {
                                return QCoreApplication::exit(0);
                        }
                }

                vate_ui(true);
                set_status(tr("All jobs finished at %1.\n\n").arg(QDateTime::currentDateTime().toString()));
                stop_button->setEnabled(false);
                currently_encoding = false;
                previous_job = Job();
                preview_size = QSize(native_width, native_height);
                emit set_preview_size(preview_size.width(), preview_size.height());
                emit_set_statid_size();
                if (shut_down_when_done_checkbox->isChecked()) {
                        return Power_Management::shut_down();
                } else {
                        return slider_moved(slider.value());
                }
        }


        stop_button->setEnabled(true);

        current_job = jobs.takeFirst();

        audio_finished = video_finished = false;

        encoding_audio = false;
        if (has_audio) {
                if (previous_job.audio.bitrate == current_job.audio.bitrate
                                && previous_job.audio.channel_layout == current_job.audio.channel_layout
                                && previous_job.audio.number_of_channels == current_job.audio.number_of_channels
                                ) { //we can reuse the audio we already encoded from the previous job (20130617)
                        audio_finished = true;
                } else {
                        emit set_target_number_of_audio_channels(current_job.audio.number_of_channels);
                        emit audio_encoding_start(current_job.audio);
                        encoding_audio = true;
                }
        } else {
                audio_finished = true;
        }

        preview_size = QSize(current_job.video.width, current_job.video.height);
        emit set_preview_size(preview_size.width(), preview_size.height());
        //emit set_statid_size(current_job.video.width_after_cropping, current_job.video.height_after_cropping);

        qDebug() << this << "frameid_queue and frameid_frame before clearing:" << frameid_queue.size() << frameid_frame.size();
        frameid_queue.clear();
        frameid_frame.clear();
        emit set_estimated_total_frames_to_encode((slider.maximum()+1) / current_job.video.f);
        if (1
                        && previous_job.name != "MQ" && current_job.name != "MQ"
                        && previous_job.video.colorspace == current_job.video.colorspace
                        && previous_job.video.number_of_frames == current_job.video.number_of_frames
                        && previous_job.video.framerate == current_job.video.framerate
                        && previous_job.video.f == current_job.video.f
                        && previous_job.video.interlaced == current_job.video.interlaced
                        && previous_job.video.tff == current_job.video.tff
                        && previous_job.video.height == current_job.video.height
                        && previous_job.video.height_after_cropping == current_job.video.height_after_cropping
                        && previous_job.video.width == current_job.video.width
                        && previous_job.video.width_after_cropping == current_job.video.width_after_cropping
                        ) {
                emit set_pass(2); //to force the video encoder to go into second pass mode (20130617)
                emit video_encoding_start(current_job.video, current_job.name, current_job.id+1);
                //the video encoder will request we call decoding_start() for the second pass (20130617)
        } else {
                emit video_encoding_start(current_job.video, current_job.name, current_job.id);

                int start = trim_start_frame;
                int stop = trim_end_frame;
                if (video_info.interlaced) {
                        start = (start+1) / 2;
                        stop = (stop+1) / 2;
                }
                emit decoding_start(current_job.id, start, stop);

                QString status_string;
                if (encoding_audio) {
                        status_string = tr("Encoding %1 video (first pass) and audio ...").arg(current_job.name);
                } else {
                        status_string = tr("Encoding %1 video (first pass) ...").arg(current_job.name);
                }
                encoding_audio = false; //this is how restart_decoder() knows whether to add "... and audio" to the status string (20130617)
                qDebug() << status_string << "job id" << current_job.id;
                set_status(status_string+"\n");
        }
}

void Yua::decoding_finished(int total_video_frames_decoded) {
        if (!currently_encoding) return;

        qDebug() << this << "decoding_finished(): total_video_frames_decoded" << total_video_frames_decoded;

        if (video_info.interlaced) total_video_frames_decoded *= 2;

        emit video_encoding_finish(total_video_frames_decoded);
        emit audio_encoding_finish();
}

void Yua::audio_encoding_finished(QString new_audio_temp_filename) {
        audio_temp_filename = new_audio_temp_filename;
        audio_finished = true;
        if (!video_finished) return;
        return mux();
}

void Yua::video_encoding_finished(QString new_video_temp_filename, int output_bitrate) {
        video_temp_filename = new_video_temp_filename;
        video_finished = true;

        while (!jobs.isEmpty()
                        && output_bitrate < current_job.video.bitrate_kbit
                        && output_bitrate < jobs.at(0).video.bitrate_kbit
                        && jobs.at(0).name != "MQ" && current_job.name != "MQ"
                        && jobs.at(0).video.colorspace == current_job.video.colorspace
                        && jobs.at(0).video.number_of_frames == current_job.video.number_of_frames
                        && jobs.at(0).video.framerate == current_job.video.framerate
                        && jobs.at(0).video.f == current_job.video.f
                        && jobs.at(0).video.interlaced == current_job.video.interlaced
                        && jobs.at(0).video.tff == current_job.video.tff
                        && jobs.at(0).video.height == current_job.video.height
                        && jobs.at(0).video.height_after_cropping == current_job.video.height_after_cropping
                        && jobs.at(0).video.width == current_job.video.width
                        && jobs.at(0).video.width_after_cropping == current_job.video.width_after_cropping
                        ) {
                set_status(tr("The %1 video was renamed to %2 because %1 was not necessary to encode.\n").arg(current_job.name).arg(jobs.at(0).name));
                current_job.name = jobs.at(0).name;
                jobs.removeFirst();
        }

        if (!audio_finished) return;
        return mux();
}

void Yua::mux() {
        if (encoding_abort) {
                encoding_abort = false;
                set_status(tr("Encode aborted by the user.\n"));

                //we definitely don't want these hanging around (20130623)
                QFile::remove(video_temp_filename);
                if (has_audio) QFile::remove(audio_temp_filename);

                return encode_jobs();
        }

        stop_button->setEnabled(false); //we don't support aborting the encode after muxing begins (20130623)

        set_status(tr("Muxing ...\n"));

        QString quality_suffix = "";
        if (current_job.name != "MQ") quality_suffix = QString("_%1").arg(current_job.name);
        temp_out_filename = QString("%1/output%2.mp4").arg(temp_dir).arg(quality_suffix);

        QStringList muxer_args;
        muxer_args
                        << "-new" << temp_out_filename
                        << "-add" << video_temp_filename
                           ;
        if (has_audio) {
                muxer_args << "-add" << audio_temp_filename;
        }

        muxer_args << "-itags" << QString("comment=yua%1 d%2 f%3 %4 %5%6 %7%8%9%10")
                      .arg(YUA_VERSION)
                      .arg(d1_button->isChecked() ? "1" : "4")
                      .arg(f1_button->isChecked() ? "1" : f2_button->isChecked() ? "2" : "3")
                      .arg(fdp_2d_button->isChecked() ? "2d" : "3d")
                      .arg((!d4_button->isChecked() || !interlaced_button->isChecked()) ? "" : no_change_button->isChecked() ? "no_change " : one_pixel_bob_button->isChecked() ? "one_pixel " : alternate_one_pixel_bob_button->isChecked() ? "alt_one_pixel " : retard_bob_button->isChecked() ? "dedeflicker " : "alt_dedeflicker ")
                      .arg(interlaced_button->isChecked() ? "interlaced" : "progressive")
                      .arg(!interlaced_button->isChecked() ? "" : tff_button->isChecked() ? "tff " : "bff ")
                      .arg(!aspect_ratio_group_box->isChecked() ? "" : standard_button->isChecked() ? "4:3 " : "16:9 ")
                      .arg(!crop_checkbox->isChecked() ? "" : QString("%1,%2,%3,%4 ")
                                                         .arg(cropping_rect.left())
                                                         .arg(cropping_rect.top())
                                                         .arg(cropping_rect.x() + cropping_rect.width())
                                                         .arg(cropping_rect.y() + cropping_rect.height())
                                                         )
                      .arg(current_job.name)
                      ;

        muxer_process.start(resource_pathname["mp4box"], muxer_args);
        muxer_process.waitForStarted();
}

void Yua::write_to_log(QString text, QString filename) {
        //
        //return;
        //

        QFile log_file(desktop_path_string + "/yua." + filename);
        log_file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream stream(&log_file);
        stream << text;
        log_file.close();
}

void Yua::process_muxer_output(QString mp4box_string_raw) {
        //qDebug() << this << "process_muxer_output():" << mp4box_string_raw;
        QStringList mp4box_strings = mp4box_string_raw.split("\r");

        foreach (QString mp4box_string, mp4box_strings) {
                if (mp4box_regex_1.indexIn(mp4box_string) > -1) {
                        add_progress(mp4box_import_progress, mp4box_regex_1.cap(1).toDouble() / 100.0);
                } else if (mp4box_regex_2.indexIn(mp4box_string) > -1) {
                        add_progress(mp4box_mux_progress, mp4box_regex_2.cap(1).toDouble() / 100.0); //this line is identical to the one two above save one character, lol (20130112)
                }
        }
}

void Yua::process_read_stderr() {
        QString mp4box_string_raw = muxer_process.readAllStandardError();
        //qDebug() << "mp4box stderr:" << mp4box_string_raw;
        //write_to_log(mp4box_string_raw, "mp4box.stderr.log");
        process_muxer_output(mp4box_string_raw);
}

void Yua::process_read_stdout() {
        QString mp4box_string_raw = muxer_process.readAllStandardOutput();
        //qDebug() << "mp4box stdout:" << mp4box_string_raw;
//        write_to_log(mp4box_string_raw, "mp4box.stdout.log");
        process_muxer_output(mp4box_string_raw);
}

void Yua::process_error(QProcess::ProcessError error) {
        error_slot(tr("Yua: mp4box_process error: %1").arg(error));
}

void Yua::show_in_finder(QString path) { //from http://lynxline.com/show-in-finder-show-in-explorer/ (20130204)
#ifdef Q_OS_MAC
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\"";
        args << "-e";
        args << "activate";
        args << "-e";
        args << "select POSIX file \""+path+"\"";
        args << "-e";
        args << "end tell";
        QProcess::startDetached("osascript", args);
#endif

#ifdef Q_WS_WIN
        QStringList args;
        args << "/select," << QDir::toNativeSeparators(path);
        QProcess::startDetached("explorer", args);
#endif

#ifdef Q_WS_X11 //don't know how to do this under linux right now (20130209)
        Q_UNUSED(path)
#endif
}

void Yua::process_finished(int, QProcess::ExitStatus) {
        QString quality_suffix = "";
        if (current_job.name != "MQ") quality_suffix = QString("_%1").arg(current_job.name);

        if (output_path.isEmpty() || !QFileInfo(output_path).exists()) {
                output_directory_choose();
                if (output_path.isEmpty() || !QFileInfo(output_path).exists()) { //they canceled the dialog or something (20130207)
                        output_path = desktop_path_string;
                        set_output_path_current_label();
                }
        }

        //if they entered e.g. Mario1_459_part5 as the output basename then they really want Mario1_459_HQ_part5.mp4 not Mario1_459_part5_HQ.mp4 (20130413)
        QString out_name = output_name_edit->text();
        QRegExp part_regex("(_part\\d+)");
        QString part = "";
        if (part_regex.indexIn(out_name) > -1) {
                part = part_regex.cap(1);
                out_name = out_name.replace(part_regex, "");
        }

        QString out_ext = "mp4";
        QString out_filename = "/"; //this should always exist (20130122)
        int i = 1;
        QString i_string = "";
        while (QFileInfo(out_filename).exists()) {
                out_filename = QString("%1/%2%3%4%5.%6").arg(output_path).arg(out_name).arg(quality_suffix).arg(part).arg(i_string).arg(out_ext);
                ++i;
                i_string = QString(" %1").arg(i);
        }

        if (!QFile::rename(temp_out_filename, out_filename)) {
                error_slot(tr("There was an error, and the output file could not be saved in the location you specified. It is currently located at: %1").arg(temp_out_filename));
                show_in_finder(temp_out_filename);
        }

        progress_bar.setValue(0);

        //these temp files are not automatically removed on program exit because i add *a.mp4 and *v.mp4 extensions to the autogenerated name because it is not unique per yua instance and i need at least two names (20130120)
        if (1) {
                if (jobs.isEmpty()) {
                        QFile::remove(video_temp_filename);
                        if (has_audio) QFile::remove(audio_temp_filename);
                }
        } else {
                show_in_finder(audio_temp_filename);
        }

        previous_job = current_job;

        return encode_jobs();
}

void Yua::vate_ui(bool boolean) {
        slider.setEnabled(boolean);
        foreach (QRadioButton *button, buttons)
                button->setEnabled(boolean);
        foreach (QCheckBox *button, checkboxes)
                button->setEnabled(boolean);
        foreach (QAction *action, actions)
                action->setEnabled(boolean);
        append_action->setEnabled(decoder.get_number_of_open_files() >= 1);
        aspect_ratio_group_box->setEnabled(boolean);
        statid_group_box->setEnabled(boolean);
        output_path_group_box->setEnabled(boolean);
}


void Yua::add_progress(Progress_Type progress_type, double amount) {
        if (progress_throttle.busy()) {
//                qDebug() << "progress_throttle busy";
                return;
        }// else qDebug() << "progress_throttle NOT busy";

        double base = 0.0;

        double x264_total_progress = 0.95;
        double mp4box_total_progress = 1.0 - x264_total_progress;

        switch (progress_type) {
        case x264_firstpass_progress:
                base = 0.0;
                amount *= x264_total_progress * .5;
                break;
        case x264_secondpass_progress:
                base = x264_total_progress * .5;
                amount *= x264_total_progress * .5;
                break;
        case mp4box_import_progress:
                base = x264_total_progress;
                amount *= mp4box_total_progress * .75;
                break;
        case mp4box_mux_progress:
                base = x264_total_progress + (mp4box_total_progress * .75);
                amount *= mp4box_total_progress * .25;
                break;
        }

        progress_bar.setValue((base + amount) * progress_bar.maximum());
        progress_throttle.done();
}

void Yua::set_status(QString text) {
        status_textedit.insertPlainText(text);
        status_textedit.ensureCursorVisible();
}

void Yua::error_slot(QString error) {
        QMessageBox::critical(this, tr("Error"), error);
}

void Yua::statid_toggled(bool boolean) {
        statid_line_1->setEnabled(boolean);
        statid_line_2->setEnabled(boolean);
        statid_line_3->setEnabled(boolean);

        sda_logo_checkbox->setEnabled(boolean);
        audio_commentary_checkbox->setEnabled(boolean);

        //slider_resume_value = slider.value();
        emit set_append_statid(boolean);
        statid_changed();
}

void Yua::trim_set_start_frame() {
        trim_start_frame = slider.value();
        if (trim_start_frame < 0) trim_start_frame = 0;
        if (trim_start_frame && trim_end_frame && trim_end_frame <= trim_start_frame) {
                trim_end_frame = trim_start_frame + 1;
        }
        qDebug() << "trim_start_frame" << trim_start_frame << "trim_end_frame" << trim_end_frame;

        make_trimpoints_label();
}

void Yua::trim_set_end_frame() {
        trim_end_frame = slider.value();
        if (trim_start_frame && trim_end_frame && trim_end_frame <= trim_start_frame) {
                trim_start_frame = trim_end_frame - 1;
        }
        qDebug() << "trim_start_frame" << trim_start_frame << "trim_end_frame" << trim_end_frame;

        make_trimpoints_label();
}

void Yua::audio_data_in(QByteArray data, qint64 audio_pts) {
//        if (!audio_first_pts_set && trim_start_frame != 0 && audio_pts != AV_NOPTS_VALUE) {
//                audio_first_pts = audio_pts;
//                audio_first_pts_set = true;
//                qDebug() << "audio_first_pts is" << audio_first_pts;
//                if (video_first_pts_set) {
//                        double difference_seconds = ((double)video_first_pts * (double)video_timebase_num / video_timebase_den)
//                                        - ((double)audio_first_pts * (double)audio_timebase_num / audio_timebase_den);
//                        qDebug()
//                                        << this << "got video first;"
//                                        << "audio_first_pts is" << audio_first_pts
//                                        << "video_first_pts is" << video_first_pts
//                                        << "floating point difference v-a" << difference_seconds;
//                        audio_first_pts += difference_seconds * (double)audio_timebase_den / audio_timebase_num;
//                        qDebug() << "now audio_first_pts is" << audio_first_pts;
//                }
//        }
//        if (audio_pts != AV_NOPTS_VALUE) {
//                audio_pts -= audio_first_pts;
//        }
        //qDebug() << this << "audio_data_out() pts" << audio_pts;
        emit audio_data_out(data, audio_pts);
}

void Yua::go_to_trim_start_frame() {
        int go_to_value = trim_start_frame;
        slider.setValue(go_to_value);
}

void Yua::go_to_trim_end_frame() {
        int go_to_value = trim_end_frame;
        if (trim_end_frame) {
                slider.setValue(go_to_value);
        } else {
                slider.setValue(slider.maximum());
        }
}

void Yua::make_trimpoints_label() {
        int image_width = progress_bar.width();
        QImage image(image_width, 1, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        if (trim_start_frame || trim_end_frame) {
                int start_pixel = (double)trim_start_frame / slider.maximum() * image_width;
                int end_pixel = (double)trim_end_frame / slider.maximum() * image_width;
                if (trim_end_frame == 0) {
                        end_pixel = image_width;
                }

                QPainter painter(&image);
                painter.setPen(QPen(Qt::black));
                //qDebug() << "drawing" << start_pixel << end_pixel;
                painter.drawPoint(start_pixel, 0);
                painter.drawPoint(end_pixel, 0);
                painter.end();

                image = image.scaled(image.width(), 10, Qt::IgnoreAspectRatio, Qt::FastTransformation);

                QPainter painter2(&image);
                painter2.setPen(QPen(Qt::blue));
                painter2.setBrush(QBrush(Qt::blue, Qt::SolidPattern));
                painter2.drawRect(start_pixel+1, 3, end_pixel-start_pixel-1, 4);
                painter2.end();
        }
        image = image.scaled(image.width(), 10, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        trimpoints_label.setPixmap(QPixmap::fromImage(image));
}

void Yua::screenshot_save() {
        if (screenshot_image.isNull()) return;

        QString out_path = desktop_path_string;
        QString out_name = "yua";
        QString out_ext = "png";
        QString out_filename = "/"; //this should always exist (20130411)
        int i = 1;
        QString i_string = "";
        while (QFileInfo(out_filename).exists()) {
                out_filename = QString("%1/%2%3.%4").arg(out_path).arg(out_name).arg(i_string).arg(out_ext);
                ++i;
                i_string = QString("%1").arg(i);
        }
        QByteArray png_string = "PNG";
        const char *png = png_string.data(); //can't pass a string constant to QtConcurrent::run() as an argument, at least under gcc 4.2 (20130412)
        QImage out_image = screenshot_image.copy(); //in case they move the slider while we're still saving the image (20130412)
        QtConcurrent::run(out_image, //this takes a while so we do it asynchronously in another thread (20130412)
                          static_cast<bool (QImage::*)(const QString &, const char *, int) const>(&QImage::save), //cast is required to distinguish between the other QImage::save() overload (20130412)
                          out_filename, png, 0);
}

void Yua::set_pretty_info(QString pretty_video_info, QString pretty_audio_info) {
        pretty_video_info_label.setFont(QFont("Courier"));
        pretty_audio_info_label.setFont(QFont("Courier"));
        pretty_video_info_label.setText(pretty_video_info);
        pretty_audio_info_label.setText(pretty_audio_info);
}

void Yua::power_management_slot() {
        QObject *sender = qobject_cast<QObject *>(QObject::sender());

        if (shut_down_action == sender) {
                return Power_Management::shut_down();
        } else if (reboot_action == sender) {
                return Power_Management::reboot();
        } else if (sleep_action == sender) {
                return Power_Management::suspend();
        } else if (hibernate_action == sender) {
                return Power_Management::hibernate();
        } else if (lock_action == sender) {
                return Power_Management::lock();
        }
}

void Yua::go_to_frame_dialog() {
        bool ok;
        int i = QInputDialog::getInt(this,
                                     tr("Go to frame"),
                                     tr("Frame:"),
                                     slider.value(), 0, slider.maximum(), 1,
                                     &ok);
        if (ok) return go_to_frame(i);
}

void Yua::go_to_frame(int frame) {
        slider.setValue(frame);
        return slider_moved(frame); //the slot may not fire on its own (20130602)
}

void Yua::statid_changed() {
        emit set_statid_line(1, statid_line_1->text());
        emit set_statid_line(2, statid_line_2->text());
        emit set_statid_line(3, statid_line_3->text());
        emit set_statid_sda_logo(sda_logo_checkbox->isChecked());
        emit set_statid_audio_commentary(audio_commentary_checkbox->isChecked());
        slider.setValue(0);
        return slider_moved(0); //the slot may not fire on its own (20130602)
}

void Yua::delete_selection() {
        if (!trim_end_frame) trim_end_frame = slider.maximum();

        int start = trim_start_frame;
        int stop = trim_end_frame;
        if (video_info.interlaced) {
                start = (start+1) / 2;
                stop = (stop+1) / 2;
        }
        int number_of_frames_to_delete = stop-start+1;
        qDebug() << this << "delete_selection(): video_info.number_of_frames" << video_info.number_of_frames << "number_of_frames_to_delete" << number_of_frames_to_delete;
        video_info.number_of_frames -= number_of_frames_to_delete;

        slider_resume_value = slider.value();
        if (trim_start_frame < slider_resume_value) {
                if (slider_resume_value > trim_end_frame) {
                        if (video_info.interlaced) {
                                slider_resume_value -= number_of_frames_to_delete*2;
                        } else {
                                slider_resume_value -= number_of_frames_to_delete;
                        }
                } else {
                        slider_resume_value = trim_start_frame+1;
                        //if (video_info.interlaced) ++slider_resume_value;
                }
        }

        emit delete_between(start, stop);
        set_slider_max();
        trim_start_frame = trim_end_frame = 0;
        make_trimpoints_label();
}

QStringList Yua::set_recommended_qualities() {
        QStringList retval;

        lq_button->setStyleSheet("QCheckBox { color: red }");
        mq_button->setStyleSheet("QCheckBox { color: red }");
        hq_button->setStyleSheet("QCheckBox { color: red }");
        retval << "l" << "m" << "h";

        iq_button->setStyleSheet("");
        xq_button->setStyleSheet("");
        if (native_height > 576+8 || (interlaced_button->isChecked() && d1_button->isChecked() && f1_button->isChecked())) {
                iq_button->setStyleSheet("QCheckBox { color: red }");
                retval << "i";
                if (native_height > 768) {
                        xq_button->setStyleSheet("QCheckBox { color: red }");
                        retval << "x";
                }
        }

        return retval;
}

void Yua::f_changed() {
        set_recommended_qualities();
}

void Yua::restore_ui() { //we have just successfully opened a file/have appended all files (20131024)
        qDebug() << "restore_ui()";
        if (cli_jobs_to_process) {
                if (cli_files_to_append) {
                        cli_files_to_append = false;
                        return append(cli_filenames_to_append);
                }
                qDebug() << "encoding_start() in 1 second";
                return QTimer::singleShot(1000, this, SLOT(encoding_start())); //give the ui time to finish firing all its signals from its initial setup and the effects to take effect - e.g. force_interlaced() (20130327)
        }
        return vate_ui(true);
}

void Yua::display_clicked(QMouseEvent *event) {
        if (!crop_checkbox->isChecked()) return;
        Webcam_Display *sender = qobject_cast<Webcam_Display *>(QObject::sender());

        QSize display_size = sender->sizeHint();
        int x = qMax(0, qMin(preview_size.width(), int(event->x() * (preview_size.width() / (double)display_size.width()))));
        int y = qMax(0, qMin(preview_size.height(), int(event->y() * (preview_size.height() / (double)display_size.height()))));

        int tolerance = 10;

        if (QEvent::MouseButtonPress == event->type()) {
                if (qAbs(cropping_rect.left() - x) < tolerance) {
                        crop_moving_left = true;
                } else if (qAbs(cropping_rect.right() - x) < tolerance) {
                        crop_moving_right = true;
                } else if (qAbs(cropping_rect.top() - y) < tolerance) {
                        crop_moving_top = true;
                } else if (qAbs(cropping_rect.bottom() - y) < tolerance) {
                        crop_moving_bottom = true;
                }
        } else if (QEvent::MouseButtonRelease == event->type()) {
                crop_moving_left = crop_moving_right = crop_moving_top = crop_moving_bottom = false;
        } else if (QEvent::MouseMove == event->type()) {
                QRect temp_cropping_rect = cropping_rect;
                if (crop_moving_left && x % 2 == 0) {
                        temp_cropping_rect.setLeft(x);
                } else if (crop_moving_right && x % 2 == 0) {
                        temp_cropping_rect.setRight(x-1);
                } else if (crop_moving_top && y % 2 == 0) {
                        temp_cropping_rect.setTop(y);
                } else if (crop_moving_bottom && y % 2 == 0) {
                        temp_cropping_rect.setBottom(y-1);
                }
                if (temp_cropping_rect.isValid()) cropping_rect = temp_cropping_rect;
                draw_preview();
        }

        if (&preview_display != sender) {
                return process_current_image();
        }
}

void Yua::crop_checkbox_clicked(bool boolean) {
        Q_UNUSED(boolean);
        return set_sizes();
}

void Yua::save_settings() {
        QHash<QString, QVariant> s;

        s["d1"] = d1_button->isChecked();
        s["d4"] = d4_button->isChecked();
        s["d4_no_change"] = no_change_button->isChecked();
        s["d4_one_pixel_bob"] = one_pixel_bob_button->isChecked();
        s["d4_alternate_one_pixel_bob"] = alternate_one_pixel_bob_button->isChecked();
        s["d4_retard_bob"] = retard_bob_button->isChecked();
        s["d4_alternate_retard_bob"] = alternate_retard_bob_button->isChecked();

        s["f1"] = f1_button->isChecked();
        s["f2"] = f2_button->isChecked();
        s["f3"] = f3_button->isChecked();

        s["2d"] = fdp_2d_button->isChecked();
        s["3d"] = fdp_3d_button->isChecked();

        s["interlaced"] = interlaced_button->isChecked();
        s["progressive"] = progressive_button->isChecked();

        s["tff"] = tff_button->isChecked();
        s["bff"] = bff_button->isChecked();

        s["aspect_ratio"] = aspect_ratio_group_box->isChecked();
        s["aspect_ratio_standard"] = standard_button->isChecked();
        s["aspect_ratio_widescreen"] = widescreen_button->isChecked();

        s["crop"] = crop_checkbox->isChecked();
        s["cropping_rect"] = cropping_rect;
        s["mono"] = mono_checkbox->isChecked();
        s["shut_down_when_done"] = shut_down_when_done_checkbox->isChecked();

        s["lq"] = lq_button->isChecked();
        s["mq"] = mq_button->isChecked();
        s["hq"] = hq_button->isChecked();
        s["iq"] = iq_button->isChecked();
        s["xq"] = xq_button->isChecked();

        s["statid"] = statid_group_box->isChecked();
        s["statid1"] = statid_line_1->text();
        s["statid2"] = statid_line_2->text();
        s["statid3"] = statid_line_3->text();
        s["statid_sda_logo"] = sda_logo_checkbox->isChecked();
        s["statid_audio_commentary"] = audio_commentary_checkbox->isChecked();

        s["output_basename"] = output_name_edit->text();


        QString filename = QFileDialog::getSaveFileName(this, tr("Save Settings File"),
                                                        settings->value("last_settings_file_location", desktop_path_string).toString(),
                                                        tr("Yua Settings files (*.ys)")
                                                        );
        if (filename.isNull()) return;
        settings->setValue("last_settings_file_location", QFileInfo(filename).absolutePath());
        QFile f(QFileInfo(filename).absoluteFilePath());
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open: %1").arg(f.fileName()));
                qDebug() << "could not open:" << f.fileName();
                return;
        }
        QDataStream stream(&f);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << s;
        f.close();
}

void Yua::load_settings() {
        QHash<QString, QVariant> s;

        QString filename = QFileDialog::getOpenFileName(this, tr("Open Settings File"),
                                                        settings->value("last_settings_file_location", desktop_path_string).toString(),
                                                        tr("Yua Settings files (*.ys)")
                                                        );
        if (filename.isNull()) return;
        settings->setValue("last_settings_file_location", QFileInfo(filename).absolutePath());
        QFile f(QFileInfo(filename).absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open: %1").arg(f.fileName()));
                qDebug() << "could not open:" << f.fileName();
                return;
        }
        QDataStream stream(&f);
        stream.setVersion(QDataStream::Qt_4_6);
        stream >> s;
        f.close();

        if (s["d1"].toBool()) d1_button->setChecked(true);
        if (s["d4"].toBool()) d4_button->setChecked(true);
        if (s["d4_no_change"].toBool()) no_change_button->setChecked(true);
        if (s["d4_one_pixel_bob"].toBool()) one_pixel_bob_button->isChecked();
        if (s["d4_alternate_one_pixel_bob"].toBool()) alternate_one_pixel_bob_button->isChecked();
        if (s["d4_retard_bob"].toBool()) retard_bob_button->isChecked();
        if (s["d4_alternate_retard_bob"].toBool()) alternate_retard_bob_button->isChecked();
        d_changed();

        if (s["f1"].toBool()) f1_button->setChecked(true);
        if (s["f2"].toBool()) f2_button->setChecked(true);
        if (s["f3"].toBool()) f3_button->setChecked(true);
        f_changed();

        if (s["2d"].toBool()) fdp_2d_button->setChecked(true);
        if (s["3d"].toBool()) fdp_3d_button->setChecked(true);

        if (s["interlaced"].toBool()) interlaced_button->setChecked(true);
        if (s["progressive"].toBool()) progressive_button->setChecked(true);
        interlaced_changed();

        if (s["tff"].toBool()) tff_button->setChecked(true);
        if (s["bff"].toBool()) bff_button->setChecked(true);
        field_dominance_changed();

        aspect_ratio_group_box->setChecked(s["aspect_ratio"].toBool());
        if (s["aspect_ratio_standard"].toBool()) standard_button->setChecked(true);
        if (s["aspect_ratio_widescreen"].toBool()) widescreen_button->setChecked(true);
        aspect_ratio_changed();

        crop_checkbox->setChecked(s["crop"].toBool());
        cropping_rect = s["cropping_rect"].toRect();
        crop_checkbox_clicked(crop_checkbox->isChecked());

        mono_checkbox->setChecked(s["mono"].toBool());
        shut_down_when_done_checkbox->setChecked(s["shut_down_when_done"].toBool());

        lq_button->setChecked(s["lq"].toBool());
        mq_button->setChecked(s["mq"].toBool());
        hq_button->setChecked(s["hq"].toBool());
        iq_button->setChecked(s["iq"].toBool());
        xq_button->setChecked(s["xq"].toBool());

        statid_group_box->setChecked(s["statid"].toBool());
        statid_line_1->setText(s["statid1"].toString());
        statid_line_2->setText(s["statid2"].toString());
        statid_line_3->setText(s["statid3"].toString());
        sda_logo_checkbox->setChecked(s["statid_sda_logo"].toBool());
        audio_commentary_checkbox->setChecked(s["statid_audio_commentary"].toBool());
        statid_toggled(statid_group_box->isChecked());

        output_name_edit->setText(s["output_basename"].toString());
}

void Yua::stop_button_pressed() {
        if (!currently_encoding) return;

        stop_button->setEnabled(false);
        jobs.clear();
        encoding_abort = true;
        emit set_encoding_abort();
}

void Yua::set_quality_checkboxes(QStringList qualities) {
        foreach (QString q, qualities) {
                if ("l" == q) lq_button->setChecked(true);
                if ("m" == q) mq_button->setChecked(true);
                if ("h" == q) hq_button->setChecked(true);
                if ("i" == q) iq_button->setChecked(true);
                if ("x" == q) xq_button->setChecked(true);
        }
}

void Yua::add_audio_commentary() {
        audio_commentary_temp_out_filename = QString("%1/ac.aac").arg(temp_dir);

        QString audio_commentary_in_filename = QFileDialog::getOpenFileName(this,
                                                        tr("Select Audio Commentary File"),
                                                        settings->value("last_file_location", desktop_path_string).toString()
                                                        );
        if (audio_commentary_in_filename.isNull()) return;
        settings->setValue("last_file_location", QFileInfo(audio_commentary_in_filename).absolutePath());
        QStringList mp4_filenames = QFileDialog::getOpenFileNames(this,
                                                              tr("Select Video Files"),
                                                              settings->value("last_file_location", desktop_path_string).toString(),
                                                              tr("Video files (*.mp4)")
                                                              );
        if (mp4_filenames.isEmpty()) return;
        settings->setValue("last_file_location", QFileInfo(mp4_filenames.at(0)).absolutePath());

        audio_commentary_mp4s_to_mux.clear();
        foreach (QString to_mux, mp4_filenames) {
                audio_commentary_mp4s_to_mux << QFileInfo(to_mux).absoluteFilePath();
        }

        QStringList ffmpeg_args;
        ffmpeg_args
                        << "-i" << QFileInfo(audio_commentary_in_filename).absoluteFilePath()
                        << "-b:a" << "32k"
                        << "-y" << audio_commentary_temp_out_filename
                           ;
        qDebug() << "ffmpeg" << ffmpeg_args.join(" ");
        set_status(tr("Encoding the audio commentary file \"%1\" ... ").arg(QFileInfo(audio_commentary_in_filename).fileName()));
        ffmpeg_process.start(resource_pathname["ffmpeg"], ffmpeg_args);
        ffmpeg_process.waitForStarted();
}

void Yua::ffmpeg_process_error(QProcess::ProcessError error) {
        set_status(tr("Encoding failed.\n"));
        QMessageBox::critical(this, tr("Error"), tr("There was an error encoding the audio commentary: %1<br><br>Is this a good .wav or .mp3 file?").arg(error));
}

void Yua::ffmpeg_process_finished(int exit_code, QProcess::ExitStatus status) {
        if (exit_code != 0 || status != QProcess::NormalExit) {
                set_status(tr("Encoding failed.\n"));
                QMessageBox::critical(this, tr("Error"), tr("There was an error encoding the audio commentary: %1<br><br>Is this a good .wav or .mp3 file?").arg(exit_code));
                return;
        }
        set_status(tr("Done.\n"));
        return mux_next_audio_commentary_mp4();
}

void Yua::mux_next_audio_commentary_mp4() {
        if (!audio_commentary_mp4s_to_mux.size()) {
                QFile::remove(audio_commentary_temp_out_filename);
                return;
        }

        QString audio_commentary_mp4_filepath = audio_commentary_mp4s_to_mux.takeFirst();

        QStringList mp4box_args;
        mp4box_args
                        << "-add" << QString("%1#trackID=1").arg(audio_commentary_mp4_filepath)
                        << "-enable" << "1"

                        << "-add" << QString("%1#trackID=2").arg(audio_commentary_mp4_filepath)
                        << "-enable" << "2"

                        << "-add" << audio_commentary_temp_out_filename
                        << "-disable" << "3"

                        << "-new" << audio_commentary_mp4_filepath
                           ;

        qDebug() << "mp4box" << mp4box_args.join(" ");
        set_status(tr("Muxing the audio commentary with the MP4 file \"%1\" ... ").arg(QFileInfo(audio_commentary_mp4_filepath).fileName()));
        audio_commentary_muxer_process.start(resource_pathname["mp4box"], mp4box_args);
        audio_commentary_muxer_process.waitForStarted();
}

void Yua::audio_commentary_muxer_process_error(QProcess::ProcessError error) {
        set_status(tr("Muxing failed.\n"));
        QMessageBox::critical(this, tr("Error"), tr("There was an error muxing the audio commentary: %1<br><br>Is this a good .mp4 file?").arg(error));
}

void Yua::audio_commentary_muxer_process_finished(int exit_code, QProcess::ExitStatus status) {
        if (exit_code != 0 || status != QProcess::NormalExit) {
                set_status(tr("Muxing failed.\n"));
                QMessageBox::critical(this, tr("Error"), tr("There was an error muxing the audio commentary: %1<br><br>Is this a good .mp4 file?").arg(exit_code));
                return;
        }
        set_status(tr("Done.\n"));
        return mux_next_audio_commentary_mp4();
}
