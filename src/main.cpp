#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>
#include <QProcess>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QTimer>
#include <QSpinBox>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpression>

class MakuToggle : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int sliderPos READ sliderPos WRITE setSliderPos)
private:
    bool m_checked = false;
    int m_sliderPos = 3;
    QPropertyAnimation *animation;
public:
    MakuToggle(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(40, 20);
        setCursor(Qt::PointingHandCursor);
        animation = new QPropertyAnimation(this, "sliderPos", this);
        animation->setDuration(200);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
    }
    bool isChecked() const { return m_checked; }
    int sliderPos() const { return m_sliderPos; }
    void setSliderPos(int pos) { m_sliderPos = pos; update(); }
signals:
    void toggled(bool checked);
protected:
    void mousePressEvent(QMouseEvent *event) override {
        Q_UNUSED(event);
        m_checked = !m_checked;
        animation->stop();
        animation->setStartValue(m_sliderPos);
        animation->setEndValue(m_checked ? 21 : 3);
        animation->start();
        emit toggled(m_checked);
    }
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath bgPath;
        bgPath.addRoundedRect(rect(), 10, 10);
        p.fillPath(bgPath, m_checked ? QColor("#58D1FA") : QColor("#1E122B"));
        if (!m_checked) { p.setPen(QPen(QColor("#3A2A4D"), 1)); p.drawPath(bgPath); }
        p.setPen(Qt::NoPen);
        p.setBrush(m_checked ? QColor("#000000") : QColor("#888888"));
        p.drawEllipse(m_sliderPos, 2, 16, 16);
    }
};

class MakuTweaker : public QMainWindow {
    Q_OBJECT
private:
    QStackedWidget *pagesWidget;
    QString osFamily = "unknown";
    QString cpuInfo;
    QString gpuInfo;
    QString ramInfo;

public:
    MakuTweaker(QWidget *parent = nullptr) : QMainWindow(parent) {
        detectOS();
        detectHardware();
        
        setWindowTitle("MakuTweaker - Linux Edition");
        resize(1150, 780);

        setStyleSheet(R"(
            QWidget { background-color: #0B0014; color: #FFFFFF; font-family: 'Segoe UI', sans-serif; }
            QLabel { background-color: transparent; color: #FFFFFF; }
            QScrollArea, QScrollArea > QWidget, QScrollArea > QWidget > QWidget { background-color: transparent; border: none; }
            QListWidget { background-color: transparent; border: none; font-size: 13px; outline: none; }
            QListWidget::item { background-color: transparent; padding: 12px 14px; border-radius: 6px; color: #DDDDDD; margin-bottom: 2px; }
            QListWidget::item:selected { background-color: #1E122B; color: #FFFFFF; border-left: 3px solid #58D1FA; font-weight: bold; }
            QListWidget::item:hover:!selected { background-color: #150A20; }
            QPushButton { background-color: #1E122B; border: 1px solid #2A1A3C; border-radius: 6px; color: #FFFFFF; padding: 8px 18px; font-size: 13px; }
            QPushButton:hover { background-color: #2A1A3C; }
            QPushButton:pressed { background-color: #3B2454; }
            QLineEdit, QSpinBox { background-color: #13081E; border: 1px solid #231636; border-radius: 8px; padding: 8px 12px; color: #FFFFFF; }
            QScrollBar:vertical { background: #0B0014; width: 8px; }
            QScrollBar::handle:vertical { background: #2A1A3C; border-radius: 4px; }
        )");

        QWidget *central = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        setCentralWidget(central);

        QWidget *sidebar = new QWidget(this);
        sidebar->setFixedWidth(300);
        QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
        sidebarLayout->setContentsMargins(10, 20, 10, 10);

        QLabel *titleLabel = new QLabel("MakuTweaker", this);
        titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; padding-left: 10px; margin-bottom: 5px;");
        sidebarLayout->addWidget(titleLabel);

        QListWidget *catList = new QListWidget(this);
        QStringList tabs = {
            "📁   Проводник и Рабочий стол", "🔄   Обновление системы", "💻   Система и ядро",
            "🎨   Персонализация", "🗑️   Удаление системного ПО", "📦   Установка ПО (Яндекс, VK, MS)",
            "🎛️   Быстрая настройка Linux", "🛠️   Продвинутые настройки", "🧩   Службы Systemd", 
            "🔑   Активация Linux", "🚀   Производительность", "⏱️   Таймер выключения", 
            "📋   Управление процессами", "ℹ️   Информация о ПК"
        };
        catList->addItems(tabs);
        sidebarLayout->addWidget(catList, 1);

        pagesWidget = new QStackedWidget(this);
        connect(catList, &QListWidget::currentRowChanged, this, &MakuTweaker::changePageAnimated);

        pagesWidget->addWidget(createDesktopPage());
        pagesWidget->addWidget(createUpdatePage());
        pagesWidget->addWidget(createSystemPage());
        pagesWidget->addWidget(createPersonalizationPage());
        pagesWidget->addWidget(createDebloatPage());
        pagesWidget->addWidget(createAppsPage()); 
        pagesWidget->addWidget(createQuickSetupPage());
        pagesWidget->addWidget(createAdvancedPage());
        pagesWidget->addWidget(createServicesPage());
        pagesWidget->addWidget(createActivationPage());
        pagesWidget->addWidget(createPerformancePage());
        pagesWidget->addWidget(createTimerPage());
        pagesWidget->addWidget(createProcessesPage());
        pagesWidget->addWidget(createPCInfoPage());
        
        catList->setCurrentRow(5);

        QWidget *footer = new QWidget(this);
        footer->setStyleSheet("border-top: 1px solid #1E122B; background-color: #08000F;");
        QHBoxLayout *footerLayout = new QHBoxLayout(footer);
        footerLayout->setContentsMargins(15, 12, 15, 12);
        
        QLineEdit *searchBar = new QLineEdit(footer);
        searchBar->setPlaceholderText("Поиск твиков, настроек и функций");
        searchBar->setFixedWidth(320);

        QLabel *linuxLabel = new QLabel("LINUX EDITION", footer);
        linuxLabel->setStyleSheet("color: #58D1FA; font-weight: bold; font-size: 12px; letter-spacing: 3px; border: none;");
        linuxLabel->setAlignment(Qt::AlignCenter);

        QPushButton *btnRestart = new QPushButton("🔄 Перезапустить GUI", footer);
        btnRestart->setStyleSheet("border: none; background: transparent; font-weight: bold;");
        connect(btnRestart, &QPushButton::clicked, this, [](){ 
            QProcess *p = new QProcess();
            p->start("bash", {"-c", "pkill bspwm || systemctl restart sddm || systemctl restart gdm"});
            connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), p, &QObject::deleteLater);
        });
        
        QPushButton *btnSettings = new QPushButton("⚙️ Настройки", footer);
        btnSettings->setStyleSheet("border: none; background: transparent; font-weight: bold;");

        footerLayout->addWidget(searchBar);
        footerLayout->addStretch();
        footerLayout->addWidget(linuxLabel);
        footerLayout->addStretch();
        footerLayout->addWidget(btnRestart);
        footerLayout->addWidget(btnSettings);

        QVBoxLayout *rightPanel = new QVBoxLayout();
        rightPanel->setContentsMargins(0,0,0,0);
        rightPanel->addWidget(pagesWidget, 1);
        rightPanel->addWidget(footer);

        mainLayout->addWidget(sidebar);
        mainLayout->addLayout(rightPanel, 1);
    }

private:
    void detectOS() {
        QFile file("/etc/os-release");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = QTextStream(&file).readAll().toLower();
            if (content.contains("arch") || content.contains("manjaro")) osFamily = "arch";
            else if (content.contains("ubuntu") || content.contains("debian")) osFamily = "debian";
            else if (content.contains("fedora") || content.contains("rhel")) osFamily = "fedora";
            else osFamily = "unknown";
        }
    }

    void detectHardware() {
        QFile fCpu("/proc/cpuinfo");
        int cores = 0; QString cpuName = "Unknown CPU";
        if (fCpu.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&fCpu);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.startsWith("model name") && cpuName == "Unknown CPU") cpuName = line.section(":", 1).trimmed();
                if (line.startsWith("processor")) cores++;
            }
        }
        cpuInfo = cpuName + " // " + QString::number(cores) + " потоков";

        QFile fRam("/proc/meminfo");
        if (fRam.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&fRam);
            long kb = in.readLine().section(QRegularExpression("\\s+"), 1, 1).toLong();
            ramInfo = QString::number(kb / 1024 / 1024 + 1) + " GB";
        } else ramInfo = "Unknown RAM";

        QProcess p;
        p.start("bash", {"-c", "lspci | grep -i 'vga\\|3d\\|display' | cut -d ':' -f 3"});
        p.waitForFinished();
        gpuInfo = QString(p.readAllStandardOutput()).trimmed();
        if (gpuInfo.isEmpty()) gpuInfo = "Unknown GPU";
    }

    QString cmdFor(const QString &archCmd, const QString &debCmd, const QString &fedCmd, const QString &defaultCmd = "") {
        if (osFamily == "arch") return archCmd;
        if (osFamily == "debian") return debCmd;
        if (osFamily == "fedora") return fedCmd;
        return defaultCmd.isEmpty() ? archCmd : defaultCmd;
    }

    QString getUniversalInstallCmd(const QString &flatpakId, const QString &aurPkg, const QString &aptPkg, const QString &dnfPkg) {
        QString arch = "sudo -u $SUDO_USER yay -S --noconfirm " + aurPkg + " || sudo -u $SUDO_USER paru -S --noconfirm " + aurPkg + " || flatpak install -y flathub " + flatpakId;
        QString deb = "apt-get install -y " + aptPkg + " || flatpak install -y flathub " + flatpakId;
        QString fed = "dnf install -y " + dnfPkg + " || flatpak install -y flathub " + flatpakId;
        return cmdFor(arch, deb, fed, "flatpak install -y flathub " + flatpakId);
    }

    void changePageAnimated(int index) {
        QWidget *w = pagesWidget->widget(index);
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(w);
        w->setGraphicsEffect(effect);
        pagesWidget->setCurrentIndex(index);
        QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(250);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    void runCmd(const QString &cmd, bool isAction = false) {
        if (isAction) {
            QString scriptPath = "/tmp/makutweaker_task.sh";
            QFile file(scriptPath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << "#!/bin/bash\n";
                out << "echo -e '\\e[1;35m== MakuTweaker Linux Edition ==\\e[0m'\n";
                out << "echo 'Выполнение: " << cmd << "'\n\n";
                out << "sudo bash -c '" << QString(cmd).replace("'", "'\\''") << "'\n\n";
                out << "echo -e '\\e[1;32mЗадача завершена! Нажми Enter, чтобы закрыть окно.\\e[0m'\n";
                out << "read\n";
                file.close();
                QProcess::execute("chmod", {"+x", scriptPath});
                
                QString termCmd = "kitty " + scriptPath + " || alacritty -e " + scriptPath + " || konsole -e " + scriptPath + " || xfce4-terminal -x " + scriptPath + " || gnome-terminal -- " + scriptPath + " || xterm -e " + scriptPath;
                
                QProcess *p = new QProcess(this);
                p->start("bash", {"-c", termCmd});
                connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), p, &QObject::deleteLater);
            }
        } else {
            QProcess *p = new QProcess(this);
            p->start("pkexec", {"bash", "-c", cmd});
            connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), p, &QObject::deleteLater);
        }
    }

    QWidget* createPageWrap(const QString &title, const QList<QWidget*> &widgets) {
        QWidget *w = new QWidget();
        w->setStyleSheet("background-color: transparent;");
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setContentsMargins(35, 30, 35, 30);
        l->setSpacing(20);
        l->setAlignment(Qt::AlignTop);
        
        QLabel *lblTitle = new QLabel(title, w);
        lblTitle->setStyleSheet("font-size: 30px; font-weight: bold; margin-bottom: 15px; color: #FFFFFF;");
        l->addWidget(lblTitle);
        
        QWidget *content = new QWidget();
        content->setStyleSheet("background-color: transparent;");
        QVBoxLayout *cl = new QVBoxLayout(content);
        cl->setContentsMargins(0,0,0,0);
        cl->setSpacing(18);
        for(auto widget : widgets) cl->addWidget(widget);
        cl->addStretch();
        
        QScrollArea *sa = new QScrollArea();
        sa->setWidgetResizable(true);
        sa->setWidget(content);
        l->addWidget(sa);
        return w;
    }

    QWidget* createToggleRow(const QString &title, const QString &cmdOn, const QString &cmdOff) {
        QWidget *w = new QWidget();
        w->setStyleSheet("background-color: transparent;");
        QHBoxLayout *l = new QHBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        QLabel *lbl = new QLabel(title, w);
        lbl->setStyleSheet("font-size: 14px; color: #FFFFFF;");
        MakuToggle *t = new MakuToggle(w);
        QLabel *st = new QLabel("Выкл.", w);
        st->setStyleSheet("font-size: 14px; margin-left: 10px; width: 45px; color: #A397B8;");
        connect(t, &MakuToggle::toggled, this, [=](bool c){
            st->setText(c ? "Вкл." : "Выкл.");
            st->setStyleSheet(c ? "font-size: 14px; margin-left: 10px; width: 45px; color: #FFFFFF; font-weight: bold;" : "font-size: 14px; margin-left: 10px; width: 45px; color: #A397B8;");
            if(c && !cmdOn.isEmpty()) runCmd(cmdOn, false);
            if(!c && !cmdOff.isEmpty()) runCmd(cmdOff, false);
        });
        l->addWidget(lbl); l->addStretch(); l->addWidget(st); l->addWidget(t);
        return w;
    }

    QWidget* createActionRow(const QString &title, const QString &btnText, const QString &cmd) {
        QWidget *w = new QWidget();
        w->setStyleSheet("background-color: transparent;");
        QHBoxLayout *l = new QHBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        QLabel *lbl = new QLabel(title, w);
        lbl->setStyleSheet("font-size: 14px; color: #FFFFFF;");
        QPushButton *btn = new QPushButton(btnText, w);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [=](){ runCmd(cmd, true); });
        l->addWidget(lbl); l->addStretch(); l->addWidget(btn);
        return w;
    }


    QWidget* createDesktopPage() {
        return createPageWrap("Проводник и Рабочий стол", {
            createToggleRow("Показывать скрытые файлы и папки", "gsettings set org.gtk.Settings.FileChooser show-hidden true", "gsettings set org.gtk.Settings.FileChooser show-hidden false"),
            createToggleRow("Блокировать создание ярлыков на рабочем столе", "chmod -w ~/Desktop", "chmod +w ~/Desktop"),
            createActionRow("Очистить кэш эскизов (Thumbnails)", "Очистить", "rm -rf ~/.cache/thumbnails/*")
        });
    }

    QWidget* createUpdatePage() {
        return createPageWrap("Обновление системы", {
            createActionRow("Очистить кэш загруженных обновлений", "Очистить", cmdFor("pacman -Sc --noconfirm", "apt clean", "dnf clean all")),
            createActionRow("Запустить поиск обновлений (Full Upgrade)", "Обновить", cmdFor("pacman -Syu", "apt update && apt upgrade -y", "dnf upgrade -y")),
            createToggleRow("Заблокировать обновление ядра системы", 
                cmdFor("sed -i 's/#IgnorePkg.*/IgnorePkg = linux linux-headers/' /etc/pacman.conf", "apt-mark hold linux-image-generic", "dnf versionlock add kernel"),
                cmdFor("sed -i 's/IgnorePkg = linux linux-headers/#IgnorePkg = linux linux-headers/' /etc/pacman.conf", "apt-mark unhold linux-image-generic", "dnf versionlock delete kernel"))
        });
    }

    QWidget* createSystemPage() {
        return createPageWrap("Система и восстановление", {
            createToggleRow("Отключить гибернацию (удаление resume)", "systemctl mask systemd-hibernate.service", "systemctl unmask systemd-hibernate.service"),
            createToggleRow("Отключить файл подкачки (Swap)", "swapoff -a", "swapon -a"),
            createToggleRow("Ускорить загрузку ОС (Скрыть GRUB)", "sed -i 's/GRUB_TIMEOUT=.*/GRUB_TIMEOUT=0/' /etc/default/grub && grub-mkconfig -o /boot/grub/grub.cfg", "sed -i 's/GRUB_TIMEOUT=.*/GRUB_TIMEOUT=5/' /etc/default/grub && grub-mkconfig -o /boot/grub/grub.cfg")
        });
    }

    QWidget* createPersonalizationPage() {
        return createPageWrap("Персонализация", {
            createToggleRow("Принудительная Темная тема (Dark Mode GTK)", "gsettings set org.gnome.desktop.interface color-scheme 'prefer-dark'", "gsettings set org.gnome.desktop.interface color-scheme 'default'")
        });
    }

    QWidget* createDebloatPage() {
        return createPageWrap("Удаление системного ПО (Debloat)", {
            createActionRow("Удалить остаточные пакеты-сироты (Orphans)", "Удалить", cmdFor("pacman -Rns $(pacman -Qtdq)", "apt autoremove -y", "dnf autoremove -y")),
            createActionRow("Очистить старые логи системы (> 10MB)", "Очистить", "journalctl --vacuum-size=10M"),
            createActionRow("Удалить неиспользуемые библиотеки Flatpak", "Удалить", "flatpak uninstall --unused -y")
        });
    }

    QWidget* createAppsPage() {
        QLabel *lbl = new QLabel("Механизм <b>«МАКС КАЧАЛО 100%»</b>: автоматический выбор источника установки.<br>Сначала идет поиск в родных пакетах и AUR (через yay/paru), при неудаче — Flatpak.");
        lbl->setStyleSheet("font-size: 13px; color: #A397B8; margin-bottom: 5px;");

        return createPageWrap("Установка ПО и Браузеров", {
            lbl,
            createActionRow("0. Установить Flatpak (Ядро для 100% установки)", "Установить", cmdFor("pacman -S --noconfirm flatpak", "apt install -y flatpak", "dnf install -y flatpak")),
            new QLabel("<b>Российские сервисы (VK / Yandex / MAX):</b>"),
            createActionRow("Установить Яндекс Браузер", "Установить", getUniversalInstallCmd("ru.yandex.Browser", "yandex-browser", "yandex-browser-stable", "yandex-browser-stable")),
            createActionRow("Установить Яндекс Диск", "Установить", getUniversalInstallCmd("ru.yandex.Disk", "yandex-disk", "yandex-disk", "yandex-disk")),
            createActionRow("Установить Яндекс Музыку (Неофиц.)", "Установить", getUniversalInstallCmd("ru.yandex.Music", "yandex-music", "yandex-music", "yandex-music")),
            createActionRow("Установить VK Мессенджер", "Установить", getUniversalInstallCmd("com.vk.VKMessenger", "vk-messenger-bin", "vk-messenger", "vk-messenger")),
            createActionRow("Установить Мессенджер МАКС (MAX)", "Установить", getUniversalInstallCmd("im.max.Messenger", "max-bin", "max-messenger", "max-messenger")),
            new QLabel("<b>Продукты Microsoft:</b>"),
            createActionRow("Установить Microsoft Edge", "Установить", getUniversalInstallCmd("com.microsoft.Edge", "microsoft-edge-stable-bin", "microsoft-edge-stable", "microsoft-edge-stable")),
            createActionRow("Установить Microsoft Teams (Неофиц.)", "Установить", getUniversalInstallCmd("com.github.IsmaelMartinez.teams_for_linux", "teams-for-linux-bin", "teams-for-linux", "teams-for-linux")),
            createActionRow("Установить Visual Studio Code", "Установить", getUniversalInstallCmd("com.visualstudio.code", "visual-studio-code-bin", "code", "code")),
            createActionRow("Установить Skype", "Установить", getUniversalInstallCmd("com.skype.Client", "skypeforlinux-bin", "skypeforlinux-stable-bin", "skypeforlinux")),
            createActionRow("Установить PowerShell", "Установить", getUniversalInstallCmd("com.microsoft.PowerShell", "powershell-bin", "powershell", "powershell"))
        });
    }

    QWidget* createQuickSetupPage() {
        return createPageWrap("Быстрая настройка Linux", {
            new QLabel("Оптимальные твики для максимальной производительности:"),
            createToggleRow("Включить Игровой Режим (Feral Gamemode)", "systemctl --user enable --now gamemoded", "systemctl --user disable --now gamemoded"),
            createToggleRow("Отключить логирование Systemd Journald", "systemctl stop systemd-journald", "systemctl start systemd-journald"),
            createToggleRow("Отключить залипание клавиш (X11)", "xset -k", "xset k")
        });
    }

    QWidget* createAdvancedPage() {
        return createPageWrap("Продвинутые настройки", {
            createToggleRow("Отключить индексирование файлов (Baloo/Tracker)", "balooctl suspend || tracker3 daemon -k", "balooctl resume || tracker3 daemon -s"),
            createToggleRow("Отключить IPv6 для ускорения сети", "sysctl net.ipv6.conf.all.disable_ipv6=1", "sysctl net.ipv6.conf.all.disable_ipv6=0"),
            createToggleRow("Отключить защиты уязвимостей CPU (Ускорение)", "echo 'GRUB_CMDLINE_LINUX_DEFAULT=\"mitigations=off\"' >> /etc/default/grub && grub-mkconfig -o /boot/grub/grub.cfg", "")
        });
    }

    QWidget* createServicesPage() {
        return createPageWrap("Службы Systemd", {
            createToggleRow("Служба печати принтеров (CUPS)", "systemctl start cups", "systemctl stop cups"),
            createToggleRow("Служба Bluetooth (Bluez)", "systemctl start bluetooth", "systemctl stop bluetooth")
        });
    }

    QWidget* createActivationPage() {
        QWidget *w = new QWidget();
        w->setStyleSheet("background-color: transparent;");
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setContentsMargins(30, 30, 30, 30);
        
        QLabel *title = new QLabel("Активация Linux", w);
        title->setStyleSheet("font-size: 30px; font-weight: bold; margin-bottom: 20px; color: #FFFFFF;");
        
        QPushButton *btnActivate = new QPushButton("Активировать Linux через FSF", w);
        btnActivate->setFixedSize(300, 45);
        btnActivate->setStyleSheet("background-color: #58D1FA; color: #000000; font-weight: bold; font-size: 15px; border-radius: 6px;");
        btnActivate->setCursor(Qt::PointingHandCursor);
        connect(btnActivate, &QPushButton::clicked, this, [](){ QDesktopServices::openUrl(QUrl("https://www.fsf.org/")); });

        QPushButton *secretBtn = new QPushButton(w);
        secretBtn->setFixedSize(30, 30);
        secretBtn->setStyleSheet("background: transparent; border: none;");
        secretBtn->setCursor(Qt::PointingHandCursor);
        connect(secretBtn, &QPushButton::clicked, this, [this](){
            QLabel *spamLabel = new QLabel(this);
            QString spamText = "МАРК АДДЕРЛИ ПИДОР ";
            QString fullText;
            for(int i=0; i<800; ++i) fullText += spamText;
            spamLabel->setText(fullText);
            spamLabel->setStyleSheet("color: #FF0055; font-size: 26px; font-weight: bold; background-color: #0B0014;");
            spamLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            spamLabel->setWordWrap(true);
            this->setCentralWidget(spamLabel);
        });

        l->addWidget(title);
        l->addWidget(btnActivate, 0, Qt::AlignCenter);
        l->addStretch();
        
        QHBoxLayout *bottomLayout = new QHBoxLayout();
        bottomLayout->addStretch();
        bottomLayout->addWidget(secretBtn);
        l->addLayout(bottomLayout);
        
        return w;
    }

    QWidget* createPerformancePage() {
        return createPageWrap("Производительность", {
            createActionRow("Включить план 'Высокая производительность'", "Применить", "cpupower frequency-set -g performance"),
            createActionRow("Включить план 'Энергосбережение'", "Применить", "cpupower frequency-set -g powersave")
        });
    }

    QWidget* createTimerPage() {
        QWidget *row = new QWidget();
        row->setStyleSheet("background-color: transparent;");
        QHBoxLayout *hl = new QHBoxLayout(row);
        QSpinBox *spin = new QSpinBox(); spin->setRange(1, 600); spin->setSuffix(" мин.");
        QPushButton *btn = new QPushButton("Завести таймер");
        QPushButton *btnCancel = new QPushButton("Отменить");
        connect(btn, &QPushButton::clicked, this, [=](){ runCmd("shutdown +" + spin->text().remove(" мин."), false); });
        connect(btnCancel, &QPushButton::clicked, this, [=](){ runCmd("shutdown -c", false); });
        hl->addWidget(new QLabel("Выключить ПК через:")); hl->addWidget(spin); hl->addWidget(btn); hl->addWidget(btnCancel); hl->addStretch();
        return createPageWrap("Таймер выключения", {row});
    }

    QWidget* createProcessesPage() {
        return createPageWrap("Управление процессами", {
            new QLabel("Блокировщик процессов (аналог MakuYan):"),
            createActionRow("Заблокировать Yandex Browser", "Блок", "killall yandex-browser-stable || pkill yandex"),
            createActionRow("Принудительно убить зависшее окно (xkill)", "xkill", "xkill")
        });
    }

    QWidget* createPCInfoPage() {
        QWidget *card = new QWidget();
        card->setStyleSheet("background-color: #130A1C; border: 1px solid #231636; border-radius: 8px;");
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(25,25,25,25);
        cl->setSpacing(12);
        
        QLabel *lCpu = new QLabel("<b>Процессор:</b> " + cpuInfo); lCpu->setStyleSheet("font-size: 15px;");
        QLabel *lGpu = new QLabel("<b>Видеокарта:</b> " + gpuInfo); lGpu->setStyleSheet("font-size: 15px;");
        QLabel *lRam = new QLabel("<b>ОЗУ:</b> " + ramInfo); lRam->setStyleSheet("font-size: 15px;");
        QLabel *lOs = new QLabel("<b>Платформа:</b> Универсальная Linux-система (" + osFamily + ")"); lOs->setStyleSheet("font-size: 15px;");
        
        cl->addWidget(lCpu); cl->addWidget(lGpu); cl->addWidget(lRam); cl->addWidget(lOs);
        return createPageWrap("Информация о ПК (Компактный режим)", {card});
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MakuTweaker window;
    window.show();
    return app.exec();
}
#include "main.moc"
