FROM python:3.10-bullseye
WORKDIR /siptv

# libs essenciais para Qt (X11/Wayland/etc.) + utilitários de debug + SQL Server driver
RUN apt-get update && apt-get install -y \
    curl \
    gnupg2 \
    apt-transport-https \
    libgl1-mesa-glx \
    libgl1-mesa-dri \
    libegl1-mesa \
    libwayland-client0 \
    libwayland-egl1-mesa \
    libx11-6 \
    libxext6 \
    libxrender1 \
    libxcb1 \
    libxkbcommon0 \
    libxkbcommon-x11-0 \
    libx11-xcb1 \
    libxcb-cursor0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-image0 \
    libxcb-icccm4 \
    libxcb-xinerama0 \
    libxcb-xfixes0 \
    libxcb-shape0 \
    libxcb-shm0 \
    libxcb-sync1 \
    libxcb-xinput0 \
    libfontconfig1 \
    libfreetype6 \
    libdbus-1-3 \
    dbus-x11 \
    x11-utils \
    x11-apps \
    unixodbc \
    unixodbc-dev \
    && rm -rf /var/lib/apt/lists/*

# Instalar ODBC Driver 17 for SQL Server
RUN curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add - \
    && curl https://packages.microsoft.com/config/debian/11/prod.list > /etc/apt/sources.list.d/mssql-release.list \
    && apt-get update \
    && ACCEPT_EULA=Y apt-get install -y msodbcsql17 mssql-tools \
    && rm -rf /var/lib/apt/lists/*

# Adicionar mssql-tools ao PATH
ENV PATH="$PATH:/opt/mssql-tools/bin"

# Copiar requirements e instalar Python packages
COPY siptv/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copiar projeto
COPY . .

CMD ["python", "siptv/lib/main/main.py"]
