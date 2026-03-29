// @ts-check

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'QuorumKit',
  tagline: 'Consensus and replicated state machines',
  url: 'https://quorumkit.org',
  baseUrl: '/',
  organizationName: 'bringhurst',
  projectName: 'quorumkit',
  onBrokenLinks: 'throw',
  i18n: {
    defaultLocale: 'en',
    locales: ['en']
  },
  markdown: {
    mermaid: true,
    hooks: {
      onBrokenMarkdownLinks: 'warn'
    }
  },
  presets: [
    [
      'classic',
      {
        docs: {
          path: 'docs',
          routeBasePath: '/docs',
          sidebarPath: require.resolve('./sidebars.js')
        },
        blog: false,
        theme: {
          customCss: require.resolve('./src/css/custom.css')
        }
      }
    ]
  ],
  themeConfig: {
    navbar: {
      items: [
        {
          to: '/docs/',
          label: 'QuorumKit',
          position: 'left'
        },
        {
          type: 'docSidebar',
          sidebarId: 'docsSidebar',
          position: 'left',
          label: 'Docs'
        },
        {
          href: 'https://github.com/bringhurst/quorumkit',
          label: 'GitHub',
          position: 'right'
        }
      ]
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Docs',
          items: [
            {
              label: 'Documentation Home',
              to: '/docs/'
            },
            {
              label: 'Architecture',
              to: '/docs/explanation/architecture-overview'
            }
          ]
        },
        {
          title: 'Project',
          items: [
            {
              label: 'GitHub',
              href: 'https://github.com/bringhurst/quorumkit'
            }
          ]
        }
      ],
      copyright: `Copyright ${new Date().getFullYear()} QuorumKit contributors`
    },
    docs: {
      sidebar: {
        autoCollapseCategories: false
      }
    }
  },
  themes: ['@docusaurus/theme-mermaid']
};

module.exports = config;
