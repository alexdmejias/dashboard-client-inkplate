# GitHub Pages Setup Guide

This guide explains how to set up GitHub Pages to host the ESP Web Tools flasher for your Inkplate Dashboard Client.

## Overview

GitHub Pages will host:
- `index.html` (flash.html renamed) - Web flashing interface
- `manifest.json` - ESP Web Tools manifest
- `firmware.bin` - Compiled firmware binary
- `firmware-info.json` - Build information

## Automatic Setup (Recommended)

Our GitHub Actions workflow automatically deploys to GitHub Pages on every push to main.

### Step 1: Enable GitHub Pages

1. Go to your repository on GitHub
2. Click **Settings** → **Pages** (in the left sidebar)
3. Under "Build and deployment":
   - **Source**: Select "GitHub Actions"
   - This allows the CI workflow to deploy automatically

That's it! The workflow will handle everything else.

### Step 2: Verify Deployment

After pushing to main:

1. Go to **Actions** tab
2. Wait for the "Build Firmware and Deploy to GitHub Pages" workflow to complete
3. Once successful, your flasher will be live at:
   ```
   https://<username>.github.io/<repository-name>/
   ```
   
   For example:
   ```
   https://alexdmejias.github.io/dashboard-client-inkplate/
   ```

### Step 3: Test the Flasher

1. Open the GitHub Pages URL in Chrome, Edge, or Opera
2. You should see the ESP Web Tools flashing interface
3. Connect your Inkplate device via USB
4. Click "Connect and Install" to test

## Manual Setup (Alternative)

If you prefer to set up GitHub Pages manually without the workflow:

### Method 1: Deploy from a Branch

1. Create a `gh-pages` branch:
   ```bash
   git checkout --orphan gh-pages
   git rm -rf .
   ```

2. Build the firmware and prepare files:
   ```bash
   # Switch back to main
   git checkout main
   
   # Build firmware
   platformio run
   
   # Create gh-pages content
   mkdir gh-pages-content
   cp .pio/build/esp32/firmware.bin gh-pages-content/
   cp flash.html gh-pages-content/index.html
   cp manifest.json gh-pages-content/
   ```

3. Switch to gh-pages and commit:
   ```bash
   git checkout gh-pages
   cp -r gh-pages-content/* .
   git add .
   git commit -m "Deploy web flasher"
   git push origin gh-pages
   ```

4. Enable GitHub Pages:
   - Go to **Settings** → **Pages**
   - **Source**: Deploy from a branch
   - **Branch**: `gh-pages` → `/ (root)`
   - Click **Save**

### Method 2: Local Development

For local testing without GitHub Pages:

1. Build the firmware:
   ```bash
   platformio run
   cp .pio/build/esp32/firmware.bin ./firmware.bin
   ```

2. Start a local web server:
   ```bash
   # Python 3
   python -m http.server 8000
   
   # Or Python 2
   python -m SimpleHTTPServer 8000
   
   # Or Node.js
   npx http-server -p 8000
   ```

3. Open in your browser:
   ```
   http://localhost:8000/flash.html
   ```

**Note**: WebSerial requires HTTPS. For local testing, browsers allow HTTP on `localhost` only.

## Customizing the Flash Page

### Update Branding

Edit `flash.html` before deployment:

```html
<h1>📱 Your Custom Title</h1>
<p>Your custom description here</p>
```

### Add Custom CSS

```html
<style>
    :root {
        --primary-color: #2196F3;
        --secondary-color: #1976D2;
    }
    .container {
        /* Your custom styles */
    }
</style>
```

### Update Links

Replace GitHub links with your repository:

```html
<li><a href="https://github.com/YOUR-USERNAME/YOUR-REPO">GitHub Repository</a></li>
```

## CI/CD Workflow Details

The `.github/workflows/build-and-deploy.yml` workflow:

### Trigger Events
- **Push to main**: Builds and deploys
- **Pull requests**: Builds only (no deployment)
- **Manual trigger**: Available in Actions tab
- **Version tags**: Creates release with firmware

### Build Process
1. Checkout code
2. Install PlatformIO
3. Build firmware using `platformio run`
4. Extract `firmware.bin` from `.pio/build/esp32/`
5. Create web directory with all needed files

### Deploy Process
1. Upload firmware as artifact (for manual download)
2. Deploy to GitHub Pages (on main branch only)
3. Create release (on version tags only)

### Workflow Permissions

Required permissions (already configured in workflow):
```yaml
permissions:
  contents: write    # For creating releases
  pages: write       # For deploying to Pages
  id-token: write    # For GitHub Pages authentication
```

## Troubleshooting

### Pages not deploying
1. Check **Actions** tab for errors
2. Verify GitHub Pages is enabled in Settings
3. Ensure "GitHub Actions" is selected as source
4. Check workflow permissions

### 404 Error on Pages URL
1. Wait 1-2 minutes after first deployment
2. Check the deployment URL in the workflow logs
3. Verify files were uploaded correctly

### WebSerial not working on Pages
- GitHub Pages uses HTTPS automatically
- Ensure you're using Chrome, Edge, or Opera
- Check browser console for errors

### Firmware not updating
1. Pages deployment caches aggressively
2. Hard refresh: `Ctrl+Shift+R` (Windows/Linux) or `Cmd+Shift+R` (Mac)
3. Clear browser cache
4. Check `firmware-info.json` for build date

## Custom Domain (Optional)

To use a custom domain:

1. Add a `CNAME` file to the web deployment:
   
   Edit `.github/workflows/build-and-deploy.yml`:
   ```yaml
   - name: Prepare firmware for web flashing
     run: |
       mkdir -p web
       # ... existing commands ...
       echo "flasher.yourdomain.com" > web/CNAME
   ```

2. Configure DNS:
   - Add a CNAME record pointing to `<username>.github.io`

3. Enable custom domain in GitHub Settings:
   - Go to **Settings** → **Pages**
   - Enter your custom domain
   - Enable "Enforce HTTPS"

## Monitoring Deployments

### View Deployment Status

```bash
# Using GitHub CLI
gh workflow view "Build Firmware and Deploy to GitHub Pages"
gh run list --workflow="build-and-deploy.yml"

# Check latest run
gh run view --log
```

### Download Artifacts

```bash
# Download firmware from latest workflow run
gh run download --name firmware
```

## Security Considerations

### HTTPS Only
- GitHub Pages serves over HTTPS automatically
- WebSerial API requires HTTPS
- Never serve firmware over HTTP in production

### Firmware Verification
- The workflow generates SHA256 checksums
- Stored in `firmware-info.json`
- Users can verify firmware integrity

### Branch Protection
Consider enabling branch protection for main:
1. Go to **Settings** → **Branches**
2. Add rule for `main` branch
3. Enable:
   - Require pull request reviews
   - Require status checks (build workflow)
   - Require conversation resolution

## Best Practices

1. **Version Tags**: Tag releases for easy tracking
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

2. **Test Before Deploy**: Test firmware via USB before deploying

3. **Document Changes**: Update README with changelog

4. **Monitor Builds**: Subscribe to workflow notifications

5. **Cache Control**: Include version info in URLs for better caching

## Example: Complete Deployment

```bash
# 1. Make changes
git checkout -b feature/my-feature
# ... make changes ...
git commit -m "Add new feature"

# 2. Create pull request (builds but doesn't deploy)
git push origin feature/my-feature
# Review in GitHub, merge to main

# 3. Automatic deployment
# Workflow runs automatically on merge to main
# Firmware is built and deployed to GitHub Pages

# 4. Create release (optional)
git checkout main
git pull
git tag v1.1.0
git push origin v1.1.0
# Workflow creates GitHub release with firmware.bin
```

## Alternative Hosting

If you prefer not to use GitHub Pages:

### Netlify
1. Connect repository to Netlify
2. Build command: `platformio run && mkdir -p dist && cp .pio/build/esp32/firmware.bin dist/ && cp flash.html dist/index.html && cp manifest.json dist/`
3. Publish directory: `dist`

### Vercel
1. Create `vercel.json`:
   ```json
   {
     "buildCommand": "platformio run && mkdir -p public && cp .pio/build/esp32/firmware.bin public/ && cp flash.html public/index.html && cp manifest.json public/",
     "outputDirectory": "public"
   }
   ```

### Self-Hosted
1. Build firmware locally
2. Upload files to your web server
3. Ensure HTTPS is enabled

## References

- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/)
- [WebSerial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
